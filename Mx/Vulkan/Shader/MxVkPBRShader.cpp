#include "MxVkPBRShader.h"
#include "../../Graphics/Texture/MxTexture.h"
#include "../MxVulkan.h"
#include "../Swapchain/MxVkSwapchain.h"
#include "../Descriptor/MxVkDescriptorSet.h"
#include "../Buffers/MxVkBuffer.h"
#include "../Pipeline/MxVkGraphicsPipelineState.h"
#include "../../Resource/MxResourceLoader.h"
#include "../../Resource/Shader/MxShaderSource.h"
#include "../Pipeline/MxVkShaderModule.h"
//#include "../../RenderAPI/MxVertexDeclaration.h"
#include "../Buffers/MxVkUniform.h"
#include "../../Component/Camera/MxCamera.h"
#include "../../Graphics/MxRenderInfo.h"
#include "../Pipeline/MxVkVertexInput.h"
#include "../../Graphics/Mesh/MxMesh.h"
#include "../Pipeline/MxVkPipeline.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"

namespace Mix {
    namespace Vulkan {

        PBRShader::PBRShader(VulkanAPI* _vulkan) :ShaderBase(_vulkan) {
            mDevice = mVulkan->getLogicalDevice();

            auto imageCount = mVulkan->getSwapchain()->imageCount();
            mCameraUbo.reserve(imageCount);
            mRenderParamUbo.reserve(imageCount);

            for (size_t i = 0; i < imageCount; ++i) {
                mCameraUbo.emplace_back(mVulkan->getAllocator(),
                                        vk::BufferUsageFlagBits::eUniformBuffer,
                                        vk::MemoryPropertyFlagBits::eHostVisible |
                                        vk::MemoryPropertyFlagBits::eHostCoherent,
                                        sizeof(Uniform::CameraUniform));
                mRenderParamUbo.emplace_back(mVulkan->getAllocator(),
                                             vk::BufferUsageFlagBits::eUniformBuffer,
                                             vk::MemoryPropertyFlagBits::eHostVisible |
                                             vk::MemoryPropertyFlagBits::eHostCoherent,
                                             sizeof(RenderParam));
            }

            loadGlobalTexture();
            buildDescriptorSetLayout();
            buildPipeline();
            buildDescriptorSet();
            buildPropertyBlock();
        }

        PBRShader::~PBRShader() {
        }

        void PBRShader::render(ArrayProxy<RenderQueueElement> _elements) {
            for (auto& element : _elements) {
                beginElement(*element.element);

                choosePipeline(*element.element->material, *element.element->mesh, element.element->submesh);
                setMaterail(*element.element->material);
                DrawMesh(*mCurrCmd, *element.element->mesh, element.element->submesh);

                endElement();
            }

        }

        void PBRShader::update(const Shader& _shader) {
            mRenderParam.lightDir = _shader.getGlobalVector("lightPos").value();
            mRenderParam.lightColor = _shader.getGlobalVector("lightColor").value();
            mRenderParam.exposure = _shader.getGlobalFloat("exposure").value();
            mRenderParam.gamma = _shader.getGlobalFloat("gamma").value();
            mRenderParam.prefilteredCubeMipLevels = _shader.getGlobalFloat("prefilteredCubeMipLevels").value();
            mRenderParam.scaleIBLAmbient = _shader.getGlobalFloat("scaleIBLAmbient").value();
        }

        void PBRShader::beginRender(const Camera& _camera) {
            mCurrFrame = mVulkan->getCurrFrame();
            mCurrCmd = &mVulkan->getCurrDrawCmd();
            mCurrVertexInput = nullptr;
            mCurrPipeline = nullptr;

            auto& cmd = mVulkan->getCurrDrawCmd();

            // update Camera
            setCamera(_camera);

            // update render param
            mRenderParamUbo[mCurrFrame].setData(&mRenderParam, sizeof(mRenderParam));
        }

        void PBRShader::endRender() {
        }

        uint32_t PBRShader::newMaterial() {
            uint32_t result = mUnusedId.back();
            mUnusedId.pop_back();
            return result;
        }

        void PBRShader::deleteMaterial(uint32_t _id) {
            mUnusedId.push_back(_id);
        }

        void PBRShader::setCamera(const Camera& _camera) {
            // update Camera
            Uniform::CameraUniform ubo;
            ubo.cameraPos = _camera.transform()->getPosition();
            ubo.viewMat = _camera.getViewMat();
            ubo.projMat = _camera.getProjMat();
            ubo.projMat[1][1] *= -1.0f;
            mCameraUbo[mCurrFrame].setData(&ubo, sizeof(ubo));

            vk::Viewport viewport(
                0.0f, 0.0f,
                _camera.getExtent().x, _camera.getExtent().y,
                0.0f, 1.0f
            );

            vk::Rect2D scissor(
                { 0, 0 },
                vk::Extent2D(_camera.getExtent().x, _camera.getExtent().y)
            );

            mCurrCmd->get().setViewport(0, viewport);
            mCurrCmd->get().setScissor(0, scissor);
        }

        void PBRShader::beginElement(const RenderElement& _element) {
            mCurrCmd->get().pushConstants<Matrix4>(mGraphicsPipelineState->getPipelineLayout(),
                                                   vk::ShaderStageFlagBits::eVertex,
                                                   0,
                                                   _element.transform->localToWorldMatrix());
            mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               mGraphicsPipelineState->getPipelineLayout(),
                                               0,
                                               mStaticDescriptorSets[mCurrFrame].get(),
                                               nullptr);
        }

        void PBRShader::endElement() {
        }

        bool PBRShader::choosePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh) {
            bool depthWrite = _material.getRenderType() != RenderType::Transparent;

            auto newVertexInput = mVulkan->getVertexInputManager().getVertexInput(*_mesh.getVertexDeclaration(), *mGraphicsPipelineState->getMeshVertexDeclaration());
            if (newVertexInput == nullptr) // This mesh is not compatiple with this pipeline
                return false;
            if (newVertexInput != mCurrVertexInput) {
                auto newPipeline = mGraphicsPipelineState->getPipeline(mVulkan->getRenderPass(), 0, newVertexInput, nullptr,_mesh.getTopology(_submesh), true, depthWrite);
                if (newPipeline != mCurrPipeline) {
                    mCurrCmd->get().bindPipeline(vk::PipelineBindPoint::eGraphics, newPipeline->get());
                    mCurrPipeline = newPipeline;
                }
                mCurrVertexInput = newVertexInput;
            }
            return true;
        }

        void PBRShader::updateTexture(Material& _material) {
            if (!_material._getChangedList().empty()) {
                std::vector<WriteDescriptorSet> writes;
                static std::string texNames[] = {
                    "colorMap",
                    "physicalDescriptorMap",
                    "normalMap",
                    "aoMap",
                    "emissiveMap"
                };

                bool textureChanged = false;
                for (uint32_t i = 0; i < 5; ++i) {
                    if (_material._getChangedList().count(texNames[i])) {
                        writes.push_back(_material.getTexture(texNames[i])->getWriteDescriptor(i, vk::DescriptorType::eCombinedImageSampler));
                        textureChanged = true;
                    }
                }

                if (textureChanged) {

                    std::swap(mMaterialDescs[0][_material._getMaterialId()], mMaterialDescs[1][_material._getMaterialId()]);
                    mMaterialDescs[0][_material._getMaterialId()].updateDescriptor(writes);
                }
            }
            _material._updated();
        }

        void PBRShader::setMaterail(Material& _material) {
            updateTexture(_material);

            MaterialParam param;
            param.baseColorFactor = _material.getVector("baseColorFactor").value();
            param.emissiveFactor = _material.getVector("emissiveFactor").value();
            param.workflow = _material.getFloat("workflow").value();
            param.diffuseFactor = _material.getVector("diffuseFactor").value();
            param.specularFactor = _material.getVector("specularFactor").value();
            param.hasBaseColorTexture = _material.getFloat("hasBaseColorTexture").value();
            param.hasPhysicalDescriptorTexture = _material.getFloat("hasPhysicalDescriptorTexture").value();
            param.hasNormalTexture = _material.getFloat("hasNormalTexture").value();
            param.hasOcclusionTexture = _material.getFloat("hasOcclusionTexture").value();
            param.hasEmissiveTexture = _material.getFloat("hasEmissiveTexture").value();
            param.metallicFactor = _material.getFloat("metallicFactor").value();
            param.roughnessFactor = _material.getFloat("roughnessFactor").value();
            param.alphaMask = _material.getFloat("alphaMask").value();
            param.alphaMaskCutoff = _material.getFloat("alphaMaskCutoff").value();

            mCurrCmd->get().pushConstants<MaterialParam>(mGraphicsPipelineState->getPipelineLayout(), vk::ShaderStageFlagBits::eFragment, sizeof(Matrix4), param);

            mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               mCurrPipeline->pipelineLayout(),
                                               1,
                                               mMaterialDescs[0][_material._getMaterialId()].get(),
                                               nullptr);
        }

        void PBRShader::buildDescriptorSetLayout() {
            mStaticParamDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mStaticParamDescriptorSetLayout->setBindings(
                {
                    {0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
                    {1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment},
                    {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
                }
            );
            mStaticParamDescriptorSetLayout->create();

            mDynamicPamramDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mDynamicPamramDescriptorSetLayout->setBindings(
                {
                    {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {3, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                    {4, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment},
                }
            );
            mDynamicPamramDescriptorSetLayout->create();
        }

        void PBRShader::buildPipeline() {
            GraphicsPipelineStateDesc desc;

            auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/pbr.vert");
            auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/mypbr.frag");
            std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
            std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

            desc.meshVertexDecl = std::make_shared<VertexDeclaration>(VertexAttribute::Position | VertexAttribute::Normal | VertexAttribute::UV0);

            desc.gpuProgram.vertex = vertShader;
            desc.gpuProgram.fragment = fragShader;

            desc.cullMode = vk::CullModeFlagBits::eBack;
            desc.frontFace = vk::FrontFace::eCounterClockwise;
            desc.polygonMode = vk::PolygonMode::eFill;

            desc.enableDepthTest = true;
            desc.enableWriteDepth = true;

            desc.descriptorSetLayouts = { mStaticParamDescriptorSetLayout,mDynamicPamramDescriptorSetLayout };
            desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

            desc.pushConstant.emplace_back(vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4));
            desc.pushConstant.emplace_back(vk::ShaderStageFlagBits::eFragment, sizeof(Matrix4), sizeof(MaterialParam));

            mGraphicsPipelineState = std::make_shared<GraphicsPipelineState>(mDevice, desc);
        }

        void PBRShader::buildDescriptorSet() {
            auto imageCount = mVulkan->getSwapchain()->imageCount();

            mDescriptorPool = std::make_shared<DescriptorPool>(mDevice);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, imageCount * 2);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eCombinedImageSampler, (5 * mDefaultMaterialCount + 3)* imageCount);
            mDescriptorPool->create((mDefaultMaterialCount + 1)*imageCount);

            mStaticDescriptorSets = mDescriptorPool->allocDescriptorSet(*mStaticParamDescriptorSetLayout, imageCount);

            // Create descriptor sets
            for (uint32_t i = 0; i < imageCount; ++i) {
                std::array<WriteDescriptorSet, 5> descriptorWrites = {
                    mCameraUbo[i].getWriteDescriptor(0, vk::DescriptorType::eUniformBuffer),
                    mRenderParamUbo[i].getWriteDescriptor(1, vk::DescriptorType::eUniformBuffer),
                    mIrradianceMap->getWriteDescriptor(2,vk::DescriptorType::eCombinedImageSampler),
                    mPrefilteredMap->getWriteDescriptor(3,vk::DescriptorType::eCombinedImageSampler),
                    mBrdfLut->getWriteDescriptor(4,vk::DescriptorType::eCombinedImageSampler)
                };

                mStaticDescriptorSets[i].updateDescriptor(descriptorWrites);
            }

            mMaterialDescs.resize(imageCount);
            for (uint32_t i = 0; i < imageCount; ++i)
                mMaterialDescs[i] = mDescriptorPool->allocDescriptorSet(*mDynamicPamramDescriptorSetLayout, mDefaultMaterialCount);
        }

        void PBRShader::buildPropertyBlock() {
            std::vector<MaterialPropertyInfo> properties = {
                MaterialPropertyInfo("baseColorFactor",                 MaterialPropertyType::VECTOR,   Vector4f::One),
                MaterialPropertyInfo("emissiveFactor",                  MaterialPropertyType::VECTOR,   Vector4f::Zero),
                MaterialPropertyInfo("workflow",                        MaterialPropertyType::FLOAT,      0.0f),
                MaterialPropertyInfo("diffuseFactor",                   MaterialPropertyType::VECTOR,   Vector4f::Zero),
                MaterialPropertyInfo("specularFactor",                  MaterialPropertyType::VECTOR,   Vector4f::Zero),
                MaterialPropertyInfo("hasBaseColorTexture",             MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("hasPhysicalDescriptorTexture",    MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("hasNormalTexture",                MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("hasOcclusionTexture",             MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("hasEmissiveTexture",              MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("metallicFactor",                  MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("roughnessFactor",                 MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("alphaMask",                       MaterialPropertyType::FLOAT,    0.0f),
                MaterialPropertyInfo("alphaMaskCutoff",                 MaterialPropertyType::FLOAT,    0.0f),

                MaterialPropertyInfo("colorMap",                        MaterialPropertyType::TEX_2D,   std::shared_ptr<Texture>()),
                MaterialPropertyInfo("physicalDescriptorMap",           MaterialPropertyType::TEX_2D,   std::shared_ptr<Texture>()),
                MaterialPropertyInfo("normalMap",                       MaterialPropertyType::TEX_2D,   std::shared_ptr<Texture>()),
                MaterialPropertyInfo("aoMap",                           MaterialPropertyType::TEX_2D,   std::shared_ptr<Texture>()),
                MaterialPropertyInfo("emissiveMap",                     MaterialPropertyType::TEX_2D,   std::shared_ptr<Texture>())

            };
            mMaterialPropertySet.insert(properties.begin(), properties.end());

            properties = {
                MaterialPropertyInfo("lightPos",MaterialPropertyType::VECTOR,Vector4f::Zero),
                MaterialPropertyInfo("lightColor",MaterialPropertyType::VECTOR,Vector4f::One),
                MaterialPropertyInfo("exposure",MaterialPropertyType::FLOAT,4.5f),
                MaterialPropertyInfo("gamma",MaterialPropertyType::FLOAT,2.2f),
                MaterialPropertyInfo("prefilteredCubeMipLevels",MaterialPropertyType::FLOAT,4.0f),
                MaterialPropertyInfo("scaleIBLAmbient",MaterialPropertyType::FLOAT,1.0f),
            };

            mShaderPropertySet.insert(properties.begin(), properties.end());

            for (auto i = 0; i < mDefaultMaterialCount; ++i)
                mUnusedId.push_back(i);
        }

        void PBRShader::loadGlobalTexture() {
            mCubeMap = ResourceLoader::Get()->load<CubeMap>("Resource/Textures/pbr/cube.ktx");
            mIrradianceMap = ResourceLoader::Get()->load<CubeMap>("Resource/Textures/pbr/Irradiance_map_2.ktx");
            mPrefilteredMap = ResourceLoader::Get()->load<CubeMap>("Resource/Textures/pbr/Prefiltered_map_2.ktx");
            mBrdfLut = ResourceLoader::Get()->load<Texture2D>("Resource/Textures/pbr/brdf.png");
        }

        //void PBRShader::genBrdfLut() {
            //const vk::Format format = vk::Format::eR16G16Sfloat;
            //const int32_t dim = 512;

            //mBrdfLut.image = std::make_shared<Image>(mVulkan->getAllocator(),
            //                                         vk::ImageType::e2D,
            //                                         vk::MemoryPropertyFlagBits::eDeviceLocal,
            //                                         vk::Extent3D(dim, dim, 1),
            //                                         format,
            //                                         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);

            //mBrdfLut.view = Image::CreateVkImageView2D(mDevice->get(),
            //                                           mBrdfLut.image->get(),
            //                                           format,
            //                                           vk::ImageAspectFlagBits::eColor);

            //vk::SamplerCreateInfo samplerCI;
            //samplerCI.magFilter = vk::Filter::eLinear;
            //samplerCI.minFilter = vk::Filter::eLinear;
            //samplerCI.mipmapMode = vk::SamplerMipmapMode::eLinear;
            //samplerCI.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            //samplerCI.addressModeV = vk::SamplerAddressMode::eClampToEdge;
            //samplerCI.addressModeW = vk::SamplerAddressMode::eClampToEdge;
            //samplerCI.minLod = 0.0f;
            //samplerCI.maxLod = 1.0f;
            //samplerCI.maxAnisotropy = 1.0f;
            //samplerCI.borderColor = vk::BorderColor::eFloatOpaqueWhite;
            //mBrdfLut.sampler = mDevice->get().createSampler(samplerCI);

            //vk::AttachmentDescription attDesc;
            //attDesc.format = format;
            //attDesc.samples = vk::SampleCountFlagBits::e1;
            //attDesc.loadOp = vk::AttachmentLoadOp::eLoad;
            //attDesc.storeOp = vk::AttachmentStoreOp::eStore;
            //attDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            //attDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            //attDesc.initialLayout = vk::ImageLayout::eUndefined;
            //attDesc.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            //vk::AttachmentReference attRef = { 0,vk::ImageLayout::eColorAttachmentOptimal };

            //vk::SubpassDescription subpassDesc;
            //subpassDesc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            //subpassDesc.colorAttachmentCount = 1;
            //subpassDesc.pColorAttachments = &attRef;

            //std::array<vk::SubpassDependency, 2> dependencies;
            //dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
            //dependencies[0].dstSubpass = 0;
            //dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
            //dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            //dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
            //dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
            //dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

            //dependencies[1].srcSubpass = 0;
            //dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
            //dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;;
            //dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;;
            //dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
            //dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
            //dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

            //vk::RenderPassCreateInfo renderPassCI;
            //renderPassCI.attachmentCount = 1;
            //renderPassCI.pAttachments = &attDesc;
            //renderPassCI.subpassCount = 1;
            //renderPassCI.pSubpasses = &subpassDesc;
            //renderPassCI.dependencyCount = 2;
            //renderPassCI.pDependencies = dependencies.data();

            //vk::RenderPass renderPass = mDevice->get().createRenderPass(renderPassCI);

            //vk::FramebufferCreateInfo framebufferCI;
            //framebufferCI.renderPass = renderPass;
            //framebufferCI.attachmentCount = 1;
            //framebufferCI.pAttachments = &mBrdfLut.view;
            //framebufferCI.width = dim;
            //framebufferCI.height = dim;
            //framebufferCI.layers = 1;

            //vk::Framebuffer framebuffer = mDevice->get().createFramebuffer(framebufferCI);

            //// Desriptors
            //DescriptorSetLayout descriptorSetLayout(mDevice);
            //descriptorSetLayout.create();

            //// Pipeline layout
            //vk::PipelineLayoutCreateInfo pipelineLayoutCI;
            //pipelineLayoutCI.setLayoutCount = 1;
            //pipelineLayoutCI.pSetLayouts = &descriptorSetLayout.get();
            //vk::PipelineLayout pipelinelayout = mDevice->get().createPipelineLayout(pipelineLayoutCI);

            //// Pipeline
            //vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCI;
            //inputAssemblyStateCI.topology = vk::PrimitiveTopology::eTriangleList;

            //vk::PipelineRasterizationStateCreateInfo rasterizationStateCI;
            //rasterizationStateCI.polygonMode = vk::PolygonMode::eFill;
            //rasterizationStateCI.cullMode = vk::CullModeFlagBits::eNone;
            //rasterizationStateCI.frontFace = vk::FrontFace::eCounterClockwise;
            //rasterizationStateCI.lineWidth = 1.0f;

            //vk::PipelineColorBlendAttachmentState blendAttachmentState;
            //blendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
            //    vk::ColorComponentFlagBits::eG |
            //    vk::ColorComponentFlagBits::eB |
            //    vk::ColorComponentFlagBits::eA;
            //blendAttachmentState.blendEnable = false;

            //vk::PipelineColorBlendStateCreateInfo colorBlendStateCI;
            //colorBlendStateCI.attachmentCount = 1;
            //colorBlendStateCI.pAttachments = &blendAttachmentState;

            //vk::PipelineDepthStencilStateCreateInfo depthStencilStateCI;
            //depthStencilStateCI.depthTestEnable = false;
            //depthStencilStateCI.depthWriteEnable = false;
            //depthStencilStateCI.depthCompareOp = vk::CompareOp::eLessOrEqual;
            //depthStencilStateCI.front = depthStencilStateCI.back;
            //depthStencilStateCI.back.compareOp = vk::CompareOp::eAlways;

            //vk::PipelineViewportStateCreateInfo viewportStateCI;
            //viewportStateCI.viewportCount = 1;
            //viewportStateCI.scissorCount = 1;

            //vk::PipelineMultisampleStateCreateInfo multisampleStateCI;
            //multisampleStateCI.rasterizationSamples = vk::SampleCountFlagBits::e1;

            //std::vector<vk::DynamicState> dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
            //vk::PipelineDynamicStateCreateInfo dynamicStateCI{};
            //dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
            //dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

            //vk::PipelineVertexInputStateCreateInfo emptyInputStateCI{};

            //// Look-up-table (from BRDF) pipeline
            //auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/genBrdfLut.vert");
            //auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/genBrdfLut.frag");
            //std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
            //std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

            //std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCIs;
            //shaderStageCIs[0].module = vertShader->get();
            //shaderStageCIs[0].stage = vk::ShaderStageFlagBits::eVertex;
            //shaderStageCIs[1].module = fragShader->get();
            //shaderStageCIs[1].stage = vk::ShaderStageFlagBits::eFragment;

            //vk::GraphicsPipelineCreateInfo pipelineCI{};
            //pipelineCI.layout = pipelinelayout;
            //pipelineCI.renderPass = renderPass;
            //pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
            //pipelineCI.pVertexInputState = &emptyInputStateCI;
            //pipelineCI.pRasterizationState = &rasterizationStateCI;
            //pipelineCI.pColorBlendState = &colorBlendStateCI;
            //pipelineCI.pMultisampleState = &multisampleStateCI;
            //pipelineCI.pViewportState = &viewportStateCI;
            //pipelineCI.pDepthStencilState = &depthStencilStateCI;
            //pipelineCI.pDynamicState = &dynamicStateCI;
            //pipelineCI.stageCount = 2;
            //pipelineCI.pStages = shaderStageCIs.data();


            //vk::Pipeline pipeline = mDevice->get().createGraphicsPipeline(nullptr, pipelineCI);

            //// Render
            //vk::ClearValue clearValues[1];
            //clearValues[0].color = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };

            //vk::RenderPassBeginInfo renderPassBeginInfo{};
            //renderPassBeginInfo.renderPass = renderPass;
            //renderPassBeginInfo.renderArea.extent.width = dim;
            //renderPassBeginInfo.renderArea.extent.height = dim;
            //renderPassBeginInfo.clearValueCount = 1;
            //renderPassBeginInfo.pClearValues = clearValues;
            //renderPassBeginInfo.framebuffer = framebuffer;

            //CommandBufferHandle cmd(mVulkan->getGraphicsCommandPool());

            //vk::RenderPassBeginInfo renderPassBI;
            //renderPassBI.renderPass = renderPass;
            //renderPassBI.framebuffer = framebuffer;
            //renderPassBI.renderArea.extent.width = dim;
            //renderPassBI.renderArea.extent.height = dim;
            //renderPassBI.clearValueCount = 1;
            //renderPassBI.pClearValues = clearValues;

            //cmd.get().begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
            //cmd.get().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

            //vk::Viewport viewport{};
            //viewport.width = (float)dim;
            //viewport.height = (float)dim;
            //viewport.minDepth = 0.0f;
            //viewport.maxDepth = 1.0f;

            //vk::Rect2D scissor{};
            //scissor.extent.width = dim;
            //scissor.extent.height = dim;

            //cmd.get().setViewport(0, viewport);
            //cmd.get().setScissor(0, scissor);
            //cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
            //cmd.get().draw(3, 1, 0, 0);
            //cmd.get().endRenderPass();
            //cmd.end();
            //cmd.submit();
            //cmd.wait();

            //mDevice->get().destroy(pipeline);
            //mDevice->get().destroy(pipelinelayout);
            //mDevice->get().destroy(renderPass);
            //mDevice->get().destroy(framebuffer);
            //mDevice->get().destroy(pipeline);

        //}

        //void PBRShader::genCubeMap() {
        //    enum Target { Irradiance = 0, Prefilteredenv = 1 };

        //    for (uint32_t target = 0; target < Prefilteredenv + 1; target++) {

        //        std::shared_ptr<CubeMap> cubeMap;

        //        auto tStart = std::chrono::high_resolution_clock::now();

        //        vk::Format format;
        //        int32_t dim;

        //        switch (target) {
        //        case Irradiance:
        //            format = vk::Format::eR32G32B32A32Sfloat;
        //            dim = 64;
        //            break;
        //        case Prefilteredenv:
        //            format = vk::Format::eR16G16B16A16Sfloat;
        //            dim = 512;
        //            break;
        //        };

        //        const uint32_t numMips = mCubeMap->mipLevels();

        //        // Create target cubemap
        //        {
        //            // Image
        //            Image image(mVulkan->getAllocator(),
        //                        vk::ImageType::e2D,
        //                        vk::MemoryPropertyFlagBits::eDeviceLocal,
        //                        vk::Extent3D(dim, dim, 1),
        //                        format,
        //                        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
        //                        numMips, 6, vk::SampleCountFlagBits::e1, vk::ImageLayout::eUndefined, vk::ImageTiling::eOptimal, vk::SharingMode::eExclusive,
        //                        vk::ImageCreateFlagBits::eCubeCompatible);

        //            // View
        //            vk::ImageView view = Image::CreateVkImageView2D(mDevice->get(),
        //                                                            image.get(),
        //                                                            format,
        //                                                            vk::ImageAspectFlagBits::eColor,
        //                                                            0, numMips,
        //                                                            0, 6);


        //            // Sampler
        //            vk::SamplerCreateInfo samplerCI;
        //            samplerCI.magFilter = vk::Filter::eLinear;
        //            samplerCI.minFilter = vk::Filter::eLinear;
        //            samplerCI.mipmapMode = vk::SamplerMipmapMode::eLinear;
        //            samplerCI.addressModeU = vk::SamplerAddressMode::eClampToEdge;
        //            samplerCI.addressModeV = vk::SamplerAddressMode::eClampToEdge;
        //            samplerCI.addressModeW = vk::SamplerAddressMode::eClampToEdge;
        //            samplerCI.minLod = 0.0f;
        //            samplerCI.maxLod = static_cast<float>(numMips);
        //            samplerCI.maxAnisotropy = 1.0f;
        //            samplerCI.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        //            vk::Sampler sampler = mDevice->get().createSampler(samplerCI);
        //        }

        //        // Color attachment
        //        vk::AttachmentDescription attDesc;
        //        attDesc.format = format;
        //        attDesc.samples = vk::SampleCountFlagBits::e1;
        //        attDesc.loadOp = vk::AttachmentLoadOp::eClear;
        //        attDesc.storeOp = vk::AttachmentStoreOp::eStore;
        //        attDesc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        //        attDesc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        //        attDesc.initialLayout = vk::ImageLayout::eUndefined;
        //        attDesc.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

        //        vk::AttachmentReference colorReference = { 0, vk::ImageLayout::eColorAttachmentOptimal };

        //        vk::SubpassDescription subpassDescription;
        //        subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        //        subpassDescription.colorAttachmentCount = 1;
        //        subpassDescription.pColorAttachments = &colorReference;

        //        // Use subpass dependencies for layout transitions
        //        std::array<vk::SubpassDependency, 2> dependencies;
        //        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        //        dependencies[0].dstSubpass = 0;
        //        dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        //        dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        //        dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
        //        dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        //        dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

        //        dependencies[1].srcSubpass = 0;
        //        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        //        dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        //        dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
        //        dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        //        dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
        //        dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

        //        // Renderpass
        //        vk::RenderPassCreateInfo renderPassCI{};
        //        renderPassCI.attachmentCount = 1;
        //        renderPassCI.pAttachments = &attDesc;
        //        renderPassCI.subpassCount = 1;
        //        renderPassCI.pSubpasses = &subpassDescription;
        //        renderPassCI.dependencyCount = 2;
        //        renderPassCI.pDependencies = dependencies.data();

        //        vk::RenderPass renderPass = mDevice->get().createRenderPass(renderPassCI);

        //        struct {
        //            std::shared_ptr<Image> image;
        //            vk::ImageView view;
        //            vk::Framebuffer framebuffer;
        //        }offscreen;

        //        // Create offscreen framebuffer
        //        {
        //            // Image
        //            offscreen.image = std::make_shared<Image>(mVulkan->getAllocator(),
        //                                                      vk::ImageType::e2D,
        //                                                      vk::MemoryPropertyFlagBits::eDeviceLocal,
        //                                                      vk::Extent3D(dim, dim, 1),
        //                                                      format,
        //                                                      vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
        //                                                      1, 1, vk::SampleCountFlagBits::e1, vk::ImageLayout::eUndefined, vk::ImageTiling::eOptimal, vk::SharingMode::eExclusive);

        //                                          // View
        //            offscreen.view = Image::CreateVkImageView2D(mDevice->get(),
        //                                                        offscreen.image->get(),
        //                                                        format,
        //                                                        vk::ImageAspectFlagBits::eColor,
        //                                                        0, 1,
        //                                                        0, 1);

        //            vk::FramebufferCreateInfo framebufferCI;
        //            framebufferCI.renderPass = renderPass;
        //            framebufferCI.attachmentCount = 1;
        //            framebufferCI.pAttachments = &offscreen.view;
        //            framebufferCI.width = dim;
        //            framebufferCI.height = dim;
        //            framebufferCI.layers = 1;
        //            offscreen.framebuffer = mDevice->get().createFramebuffer(framebufferCI);

        //            CommandBufferHandle cmd(mVulkan->getGraphicsCommandPool());

        //            vk::ImageMemoryBarrier imageMemoryBarrier;
        //            imageMemoryBarrier.image = offscreen.image->get();
        //            imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
        //            imageMemoryBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
        //            imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits();
        //            imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        //            imageMemoryBarrier.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
        //            cmd.get().pipelineBarrier(vk::PipelineStageFlagBits::eAllCommands,
        //                                      vk::PipelineStageFlagBits::eAllCommands,
        //                                      vk::DependencyFlags(),
        //                                      nullptr, nullptr, imageMemoryBarrier);
        //            cmd.submit();
        //            cmd.wait();
        //        }

        //        // Descriptors
        //        DescriptorSetLayout descriptorSetLayout(mDevice);
        //        descriptorSetLayout.addBinding({ 0,vk::DescriptorType::eCombinedImageSampler,1,vk::ShaderStageFlagBits::eFragment });
        //        descriptorSetLayout.create();

        //        // Descriptor Pool
        //        DescriptorPool descriptorPool(mDevice);
        //        descriptorPool.addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1);
        //        descriptorPool.create(2);

        //        auto descriptorSet = descriptorPool.allocDescriptorSet(descriptorSetLayout);

        //        auto write = mCubeMap->getWriteDescriptor(0, vk::DescriptorType::eCombinedImageSampler);
        //        descriptorSet.updateDescriptor(write);

        //        struct PushBlockIrradiance {
        //            glm::mat4 mvp;
        //            float deltaPhi = (2.0f * float(M_PI)) / 180.0f;
        //            float deltaTheta = (0.5f * float(M_PI)) / 64.0f;
        //        } pushBlockIrradiance;

        //        struct PushBlockPrefilterEnv {
        //            glm::mat4 mvp;
        //            float roughness;
        //            uint32_t numSamples = 32u;
        //        } pushBlockPrefilterEnv;

        //        // Pipeline layout

        //        vk::PushConstantRange pushConstantRange;
        //        pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;

        //        switch (target) {
        //        case Irradiance:
        //            pushConstantRange.size = sizeof(PushBlockIrradiance);
        //            break;
        //        case Prefilteredenv:
        //            pushConstantRange.size = sizeof(PushBlockPrefilterEnv);
        //            break;
        //        };

        //        vk::PipelineLayoutCreateInfo pipelineLayoutCI;
        //        pipelineLayoutCI.setLayoutCount = 1;
        //        pipelineLayoutCI.pSetLayouts = &descriptorSetLayout.get();
        //        pipelineLayoutCI.pushConstantRangeCount = 1;
        //        pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;
        //        auto pipelinelayout = mDevice->get().createPipelineLayout(pipelineLayoutCI);

        //        // Pipeline
        //        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
        //        inputAssemblyStateCI.topology = vk::PrimitiveTopology::eTriangleList;

        //        vk::PipelineRasterizationStateCreateInfo rasterizationStateCI{};
        //        rasterizationStateCI.polygonMode = vk::PolygonMode::eFill;
        //        rasterizationStateCI.cullMode = vk::CullModeFlagBits::eNone;
        //        rasterizationStateCI.frontFace = vk::FrontFace::eCounterClockwise;
        //        rasterizationStateCI.lineWidth = 1.0f;

        //        vk::PipelineColorBlendAttachmentState blendAttachmentState;
        //        blendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR |
        //            vk::ColorComponentFlagBits::eG |
        //            vk::ColorComponentFlagBits::eB |
        //            vk::ColorComponentFlagBits::eA;
        //        blendAttachmentState.blendEnable = false;

        //        vk::PipelineColorBlendStateCreateInfo colorBlendStateCI{};
        //        colorBlendStateCI.attachmentCount = 1;
        //        colorBlendStateCI.pAttachments = &blendAttachmentState;

        //        vk::PipelineDepthStencilStateCreateInfo depthStencilStateCI{};
        //        depthStencilStateCI.depthTestEnable = false;
        //        depthStencilStateCI.depthWriteEnable = false;
        //        depthStencilStateCI.depthCompareOp = vk::CompareOp::eLessOrEqual;
        //        depthStencilStateCI.front = depthStencilStateCI.back;
        //        depthStencilStateCI.back.compareOp = vk::CompareOp::eAlways;

        //        vk::PipelineViewportStateCreateInfo viewportStateCI{};
        //        viewportStateCI.viewportCount = 1;
        //        viewportStateCI.scissorCount = 1;

        //        vk::PipelineMultisampleStateCreateInfo multisampleStateCI{};
        //        multisampleStateCI.rasterizationSamples = vk::SampleCountFlagBits::e1;

        //        std::vector<vk::DynamicState> dynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        //        vk::PipelineDynamicStateCreateInfo dynamicStateCI{};
        //        dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
        //        dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

        //        // Vertex input state
        //        vk::VertexInputBindingDescription vertexInputBinding = { 0, sizeof(vk::glTF::Model::Vertex), vk::VertexInputRate::eVertex };
        //        vk::VertexInputAttributeDescription vertexInputAttribute = { 0, 0, vk::_FORMAT_R32G32B32_SFLOAT, 0 };

        //        vk::PipelineVertexInputStateCreateInfo vertexInputStateCI{};
        //        vertexInputStateCI.vertexBindingDescriptionCount = 1;
        //        vertexInputStateCI.pVertexBindingDescriptions = &vertexInputBinding;
        //        vertexInputStateCI.vertexAttributeDescriptionCount = 1;
        //        vertexInputStateCI.pVertexAttributeDescriptions = &vertexInputAttribute;

        //        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

        //        vk::GraphicsPipelineCreateInfo pipelineCI{};
        //        pipelineCI.layout = pipelinelayout;
        //        pipelineCI.renderPass = renderpass;
        //        pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
        //        pipelineCI.pVertexInputState = &vertexInputStateCI;
        //        pipelineCI.pRasterizationState = &rasterizationStateCI;
        //        pipelineCI.pColorBlendState = &colorBlendStateCI;
        //        pipelineCI.pMultisampleState = &multisampleStateCI;
        //        pipelineCI.pViewportState = &viewportStateCI;
        //        pipelineCI.pDepthStencilState = &depthStencilStateCI;
        //        pipelineCI.pDynamicState = &dynamicStateCI;
        //        pipelineCI.stageCount = 2;
        //        pipelineCI.pStages = shaderStages.data();
        //        pipelineCI.renderPass = renderpass;

        //        shaderStages[0] = loadShader(device, "filtercube.vert.spv", vk::_SHADER_STAGE_VERTEX_BIT);
        //        switch (target) {
        //        case Irradiance:
        //            shaderStages[1] = loadShader(device, "irradiancecube.frag.spv", vk::_SHADER_STAGE_FRAGMENT_BIT);
        //            break;
        //        case Prefilteredenv:
        //            shaderStages[1] = loadShader(device, "prefilterenvmap.frag.spv", vk::_SHADER_STAGE_FRAGMENT_BIT);
        //            break;
        //        };
        //        vk::Pipeline pipeline;
        //        vk::_CHECK_RESULT(vk::CreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipeline));
        //        for (auto shaderStage : shaderStages) {
        //            vk::DestroyShaderModule(device, shaderStage.module, nullptr);
        //        }

        //        // Render cubemap
        //        vk::ClearValue clearValues[1];
        //        clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 0.0f } };

        //        vk::RenderPassBeginInfo renderPassBeginInfo{};
        //        renderPassBeginInfo.sType = vk::_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        //        renderPassBeginInfo.renderPass = renderpass;
        //        renderPassBeginInfo.framebuffer = offscreen.framebuffer;
        //        renderPassBeginInfo.renderArea.extent.width = dim;
        //        renderPassBeginInfo.renderArea.extent.height = dim;
        //        renderPassBeginInfo.clearValueCount = 1;
        //        renderPassBeginInfo.pClearValues = clearValues;

        //        std::vector<glm::mat4> matrices = {
        //            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        //            glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        //            glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        //            glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        //            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        //            glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        //        };

        //        vk::CommandBuffer cmdBuf = mDevice->get().create(vk::_COMMAND_BUFFER_LEVEL_PRIMARY, false);

        //        vk::Viewport viewport{};
        //        viewport.width = (float)dim;
        //        viewport.height = (float)dim;
        //        viewport.minDepth = 0.0f;
        //        viewport.maxDepth = 1.0f;

        //        vk::Rect2D scissor{};
        //        scissor.extent.width = dim;
        //        scissor.extent.height = dim;

        //        vk::ImageSubresourceRange subresourceRange{};
        //        subresourceRange.aspectMask = vk::_IMAGE_ASPECT_COLOR_BIT;
        //        subresourceRange.baseMipLevel = 0;
        //        subresourceRange.levelCount = numMips;
        //        subresourceRange.layerCount = 6;

        //        // Change image layout for all cubemap faces to transfer destination
        //        {
        //            vulkanDevice->beginCommandBuffer(cmdBuf);
        //            vk::ImageMemoryBarrier imageMemoryBarrier{};
        //            imageMemoryBarrier.sType = vk::_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //            imageMemoryBarrier.image = cubemap.image;
        //            imageMemoryBarrier.oldLayout = vk::_IMAGE_LAYOUT_UNDEFINED;
        //            imageMemoryBarrier.newLayout = vk::_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        //            imageMemoryBarrier.srcAccessMask = 0;
        //            imageMemoryBarrier.dstAccessMask = vk::_ACCESS_TRANSFER_WRITE_BIT;
        //            imageMemoryBarrier.subresourceRange = subresourceRange;
        //            vk::CmdPipelineBarrier(cmdBuf, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        //            vulkanDevice->flushCommandBuffer(cmdBuf, queue, false);
        //        }

        //        for (uint32_t m = 0; m < numMips; m++) {
        //            for (uint32_t f = 0; f < 6; f++) {

        //                vulkanDevice->beginCommandBuffer(cmdBuf);

        //                viewport.width = static_cast<float>(dim * std::pow(0.5f, m));
        //                viewport.height = static_cast<float>(dim * std::pow(0.5f, m));
        //                vk::CmdSetViewport(cmdBuf, 0, 1, &viewport);
        //                vk::CmdSetScissor(cmdBuf, 0, 1, &scissor);

        //                // Render scene from cube face's point of view
        //                vk::CmdBeginRenderPass(cmdBuf, &renderPassBeginInfo, vk::_SUBPASS_CONTENTS_INLINE);

        //                // Pass parameters for current pass using a push constant block
        //                switch (target) {
        //                case Irradiance:
        //                    pushBlockIrradiance.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
        //                    vk::CmdPushConstants(cmdBuf, pipelinelayout, vk::_SHADER_STAGE_VERTEX_BIT | vk::_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlockIrradiance), &pushBlockIrradiance);
        //                    break;
        //                case Prefilteredenv:
        //                    pushBlockPrefilterEnv.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
        //                    pushBlockPrefilterEnv.roughness = (float)m / (float)(numMips - 1);
        //                    vk::CmdPushConstants(cmdBuf, pipelinelayout, vk::_SHADER_STAGE_VERTEX_BIT | vk::_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushBlockPrefilterEnv), &pushBlockPrefilterEnv);
        //                    break;
        //                };

        //                vk::CmdBindPipeline(cmdBuf, vk::_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        //                vk::CmdBindDescriptorSets(cmdBuf, vk::_PIPELINE_BIND_POINT_GRAPHICS, pipelinelayout, 0, 1, &descriptorset, 0, NULL);

        //                vk::DeviceSize offsets[1] = { 0 };

        //                models.skybox.draw(cmdBuf);

        //                vk::CmdEndRenderPass(cmdBuf);

        //                vk::ImageSubresourceRange subresourceRange = { vk::_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        //                subresourceRange.aspectMask = vk::_IMAGE_ASPECT_COLOR_BIT;
        //                subresourceRange.baseMipLevel = 0;
        //                subresourceRange.levelCount = numMips;
        //                subresourceRange.layerCount = 6;

        //                {
        //                    vk::ImageMemoryBarrier imageMemoryBarrier{};
        //                    imageMemoryBarrier.sType = vk::_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //                    imageMemoryBarrier.image = offscreen.image;
        //                    imageMemoryBarrier.oldLayout = vk::_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //                    imageMemoryBarrier.newLayout = vk::_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //                    imageMemoryBarrier.srcAccessMask = vk::_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        //                    imageMemoryBarrier.dstAccessMask = vk::_ACCESS_TRANSFER_READ_BIT;
        //                    imageMemoryBarrier.subresourceRange = { vk::_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        //                    vk::CmdPipelineBarrier(cmdBuf, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        //                }

        //                // Copy region for transfer from framebuffer to cube face
        //                vk::ImageCopy copyRegion{};

        //                copyRegion.srcSubresource.aspectMask = vk::_IMAGE_ASPECT_COLOR_BIT;
        //                copyRegion.srcSubresource.baseArrayLayer = 0;
        //                copyRegion.srcSubresource.mipLevel = 0;
        //                copyRegion.srcSubresource.layerCount = 1;
        //                copyRegion.srcOffset = { 0, 0, 0 };

        //                copyRegion.dstSubresource.aspectMask = vk::_IMAGE_ASPECT_COLOR_BIT;
        //                copyRegion.dstSubresource.baseArrayLayer = f;
        //                copyRegion.dstSubresource.mipLevel = m;
        //                copyRegion.dstSubresource.layerCount = 1;
        //                copyRegion.dstOffset = { 0, 0, 0 };

        //                copyRegion.extent.width = static_cast<uint32_t>(viewport.width);
        //                copyRegion.extent.height = static_cast<uint32_t>(viewport.height);
        //                copyRegion.extent.depth = 1;

        //                vk::CmdCopyImage(
        //                    cmdBuf,
        //                    offscreen.image,
        //                    vk::_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        //                    cubemap.image,
        //                    vk::_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        //                    1,
        //                    &copyRegion);

        //                {
        //                    vk::ImageMemoryBarrier imageMemoryBarrier{};
        //                    imageMemoryBarrier.sType = vk::_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //                    imageMemoryBarrier.image = offscreen.image;
        //                    imageMemoryBarrier.oldLayout = vk::_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        //                    imageMemoryBarrier.newLayout = vk::_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //                    imageMemoryBarrier.srcAccessMask = vk::_ACCESS_TRANSFER_READ_BIT;
        //                    imageMemoryBarrier.dstAccessMask = vk::_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        //                    imageMemoryBarrier.subresourceRange = { vk::_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        //                    vk::CmdPipelineBarrier(cmdBuf, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        //                }

        //                vulkanDevice->flushCommandBuffer(cmdBuf, queue, false);
        //            }
        //        }

        //        {
        //            vulkanDevice->beginCommandBuffer(cmdBuf);
        //            vk::ImageMemoryBarrier imageMemoryBarrier{};
        //            imageMemoryBarrier.sType = vk::_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //            imageMemoryBarrier.image = cubemap.image;
        //            imageMemoryBarrier.oldLayout = vk::_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        //            imageMemoryBarrier.newLayout = vk::_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //            imageMemoryBarrier.srcAccessMask = vk::_ACCESS_TRANSFER_WRITE_BIT;
        //            imageMemoryBarrier.dstAccessMask = vk::_ACCESS_HOST_WRITE_BIT | vk::_ACCESS_TRANSFER_WRITE_BIT;
        //            imageMemoryBarrier.subresourceRange = subresourceRange;
        //            vk::CmdPipelineBarrier(cmdBuf, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, vk::_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        //            vulkanDevice->flushCommandBuffer(cmdBuf, queue, false);
        //        }


        //        vk::DestroyRenderPass(device, renderpass, nullptr);
        //        vk::DestroyFramebuffer(device, offscreen.framebuffer, nullptr);
        //        vk::FreeMemory(device, offscreen.memory, nullptr);
        //        vk::DestroyImageView(device, offscreen.view, nullptr);
        //        vk::DestroyImage(device, offscreen.image, nullptr);
        //        vk::DestroyDescriptorPool(device, descriptorpool, nullptr);
        //        vk::DestroyDescriptorSetLayout(device, descriptorsetlayout, nullptr);
        //        vk::DestroyPipeline(device, pipeline, nullptr);
        //        vk::DestroyPipelineLayout(device, pipelinelayout, nullptr);

        //        cubemap.descriptor.imageView = cubemap.view;
        //        cubemap.descriptor.sampler = cubemap.sampler;
        //        cubemap.descriptor.imageLayout = vk::_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //        cubemap.device = vulkanDevice;

        //        switch (target) {
        //        case Irradiance:
        //            textures.irradianceCube = cubemap;
        //            break;
        //        case Prefilteredenv:
        //            textures.prefilteredCube = cubemap;
        //            shaderValuesParams.prefilteredCubeMipLevels = static_cast<float>(numMips);
        //            break;
        //        };

        //        auto tEnd = std::chrono::high_resolution_clock::now();
        //        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        //        std::cout << "Generating cube map with " << numMips << " mip levels took " << tDiff << " ms" << std::endl;
        //    }
        //}
    }
}

#include "MxVkBallShader.h"
#include "../Buffers/MxVkUniform.h"
#include "../Swapchain/MxVkSwapchain.h"
#include "../Pipeline/MxVkRenderPass.h"
#include "../Pipeline/MxVkPipeline.h"
#include "../FrameBuffer/MxVkFramebuffer.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"
#include "../../Resource/Shader/MxShaderSource.h"
#include "../../Graphics/Texture/MxTexture.h"
#include "../Pipeline/MxVkPipelineFactory.h"
#include "../../GameObject/MxGameObject.h"
#include "../../Component/Camera/MxCamera.h"
#include "../../Component/Renderer/MxRenderer.h"
#include "../../Resource/MxResourceLoader.h"
#include "../Pipeline/MxVkShaderModule.h"
#include "../MxVulkan.h"
#include "../Pipeline/MxVkVertexInput.h"
#include "../../Graphics/MxRenderInfo.h"

namespace Mix {
    namespace Vulkan {
        BallShader::BallShader(VulkanAPI* _vulkan) :ShaderBase(_vulkan) {
            mDevice = mVulkan->getLogicalDevice();

            auto imageCount = mVulkan->getSwapchain()->imageCount();
            mCameraUniforms.reserve(imageCount);
            mInstanceBuffer.resize(imageCount);


            for (size_t i = 0; i < imageCount; ++i) {
                mCameraUniforms.emplace_back(mVulkan->getAllocator(),
                                             vk::BufferUsageFlagBits::eUniformBuffer,
                                             vk::MemoryPropertyFlagBits::eHostVisible |
                                             vk::MemoryPropertyFlagBits::eHostCoherent,
                                             sizeof(Uniform::CameraUniform));
            }

            buildDescriptorSetLayout();
            buildPipeline();
            buildDescriptorSet();
            buildPropertyBlock();
        }

        BallShader::~BallShader() {
        }

        void BallShader::beginRender(const Camera& _camera) {
            mCurrFrame = mVulkan->getCurrFrame();
            mCurrCmd = &mVulkan->getCurrDrawCmd();
            mCurrVertexInput = nullptr;
            mCurrPipeline = nullptr;

            auto& cmd = mVulkan->getCurrDrawCmd();

            // update Camera
            setCamera(_camera);
        }

        void BallShader::endRender() {
            mNextInstanceBufferIdx = 0;
            // mDynamicUniform[mCurrFrame].reset();
        }

        void BallShader::setCamera(const Camera& _camera) {
            // update Camera
            Uniform::CameraUniform ubo;
            ubo.cameraPos = _camera.transform()->getPosition();
            ubo.viewMat = _camera.getViewMat();
            ubo.projMat = _camera.getProjMat();
            ubo.projMat[1][1] *= -1.0f;
            mCameraUniforms[mCurrFrame].setData(&ubo, sizeof(ubo));

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

        void BallShader::beginElement(const RenderElement& _element) {
            mCurrCmd->get().pushConstants<Matrix4>(mGraphicsPipeline->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, _element.transform->localToWorldMatrix());
            mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               mGraphicsPipeline->getPipelineLayout(),
                                               0,
                                               mStaticDescriptorSets[mCurrFrame].get(),
                                               nullptr);
        }

        void BallShader::endElement() {
        }

        bool BallShader::choosePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh) {
            bool depthWrite = _material.getRenderType() != RenderType::Transparent;

            auto newVertexInput = mVulkan->getVertexInputManager().getVertexInput(*_mesh.getVertexDeclaration(), *mGraphicsPipeline->getMeshVertexDeclaration());
            if (newVertexInput == nullptr) // This mesh is not compatiple with this pipeline
                return false;
            auto newPipeline = mGraphicsPipeline->getPipeline(mVulkan->getRenderPass(), 0, newVertexInput, nullptr, _mesh.getTopology(_submesh), true, depthWrite);
            if (newPipeline != mCurrPipeline) {
                mCurrCmd->get().bindPipeline(vk::PipelineBindPoint::eGraphics, newPipeline->get());
                mCurrPipeline = newPipeline;
            }
            mCurrVertexInput = newVertexInput;

            return true;
        }

        bool BallShader::chooseInstancePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh) {
            bool depthWrite = _material.getRenderType() != RenderType::Transparent;

            auto newVertexInput = mVulkan->getVertexInputManager().getVertexInput(*_mesh.getVertexDeclaration(), *mInstanceGraphicsPipeline->getMeshVertexDeclaration());
            auto extraInput = mVulkan->getVertexInputManager().getVertexInput(*mInstanceGraphicsPipeline->getExtraVertexDeclaration(), *mInstanceGraphicsPipeline->getExtraVertexDeclaration());
            if (newVertexInput == nullptr) // This mesh is not compatiple with this pipeline
                return false;
            auto newPipeline = mInstanceGraphicsPipeline->getPipeline(mVulkan->getRenderPass(), 0, newVertexInput, extraInput, _mesh.getTopology(_submesh), true, depthWrite);
            if (newPipeline != mCurrPipeline) {
                mCurrCmd->get().bindPipeline(vk::PipelineBindPoint::eGraphics, newPipeline->get());
                mCurrPipeline = newPipeline;
            }
            mCurrVertexInput = newVertexInput;
            return true;
        }

        std::shared_ptr<Buffer> BallShader::getUnusedInstanceBuffer(uint32_t _size) {
            if (mNextInstanceBufferIdx >= mInstanceBuffer[mCurrFrame].size())
                mInstanceBuffer[mCurrFrame].emplace_back();

            auto& buffer = mInstanceBuffer[mCurrFrame][mNextInstanceBufferIdx];

            if (!buffer || buffer->size() < _size)
                buffer = std::make_shared<Buffer>(mVulkan->getAllocator(),
                                                  vk::BufferUsageFlagBits::eVertexBuffer,
                                                  vk::MemoryPropertyFlagBits::eHostVisible |
                                                  vk::MemoryPropertyFlagBits::eHostCoherent,
                                                  _size);

            ++mNextInstanceBufferIdx;
            return buffer;
        }

        void BallShader::updateMaterial(Material& _material) {
            _material._updated();
        }

        void BallShader::setMaterail(Material& _material) {
            updateMaterial(_material);
            BallShaderParameter para;
            para.mainColor = _material.getVector("mainColor").value();
            para.rimColor = _material.getVector("rimColor").value();
            para.rimPower = _material.getFloat("rimPower").value();
            para.rimIntensity = _material.getFloat("rimIntensity").value();

            mCurrCmd->get().pushConstants<BallShaderParameter>(mCurrPipeline->pipelineLayout(),
                                                               vk::ShaderStageFlagBits::eFragment,
                                                               0,
                                                               para);
        }

        void BallShader::render(ArrayProxy<RenderQueueElement> _elements) {

            uint32_t matId;
            auto lower = _elements.begin();
            auto upper = lower + 1;

            while (lower != _elements.end()) {
                matId = lower->element->material->_getMaterialId();
                while (upper != _elements.end() && matId == upper->element->material->_getMaterialId())
                    ++upper;

                if (lower->element->material->getGPUInstancing()) {
                    auto& mesh = *lower->element->mesh;
                    auto& material = *lower->element->material;
                    uint32_t instanceCount = upper - lower;

                    chooseInstancePipeline(*lower->element->material, *lower->element->mesh, lower->element->submesh);

                    mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                       mInstanceGraphicsPipeline->getPipelineLayout(),
                                                       0,
                                                       mStaticDescriptorSets[mCurrFrame].get(),
                                                       nullptr);

                    auto instanceBuffer = getUnusedInstanceBuffer(instanceCount * sizeof(Matrix4));

                    // Copy instance data
                    auto p = reinterpret_cast<Matrix4*>(instanceBuffer->rawPtr());

                    for (auto it = lower; it != upper; ++it) {
                        memcpy(p, &(it->element->transform->localToWorldMatrix()), sizeof(Matrix4));
                        ++p;
                    }

                    setMaterail(material);

                            // Draw
                    mCurrCmd->get().bindVertexBuffers(0, mesh.getVertexBuffer()->get(), { 0 });
                    mCurrCmd->get().bindVertexBuffers(1, instanceBuffer->get(), { 0 });
                    mCurrCmd->get().bindIndexBuffer(mesh.getIndexBuffer()->get(),
                                                    0,
                                                    mesh.indexFormat() == IndexFormat::UInt16 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);

                    mCurrCmd->get().drawIndexed(mesh.getSubMesh()[lower->element->submesh].indexCount,
                                                instanceCount,
                                                mesh.getSubMesh()[lower->element->submesh].firstIndex,
                                                mesh.getSubMesh()[lower->element->submesh].baseVertex,
                                                0);

                    lower = upper;
                }
                /*else {
                    while (lower != upper) {
                        beginElement(*lower->element);
                        setMaterail(*lower->element->material);
                        choosePipeline(*lower->element->material, *lower->element->mesh, lower->element->submesh);
                        DrawMesh(*mCurrCmd, *lower->element->mesh, lower->element->submesh);
                        endElement();

                        ++lower;
                    }
                }*/
            }

        }

        void BallShader::update(const Shader& _shader) {
        }

        uint32_t BallShader::newMaterial() {
            uint32_t result = mUnusedId.back();
            mUnusedId.pop_back();
            return result;
        }

        void BallShader::deleteMaterial(uint32_t _id) {
            mUnusedId.push_back(_id);
        }

        void BallShader::buildDescriptorSetLayout() {
            mStaticParamDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mStaticParamDescriptorSetLayout->setBindings(
                {
                    {0,vk::DescriptorType::eUniformBuffer,1,vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment}
                }
            );
            mStaticParamDescriptorSetLayout->create();

        }

        void BallShader::buildDescriptorSet() {
            auto imageCount = mVulkan->getSwapchain()->imageCount();

            mDescriptorPool = std::make_shared<DescriptorPool>(mDevice);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, imageCount);
            mDescriptorPool->create((mDefaultMaterialCount + 1)*imageCount);


            mStaticDescriptorSets = mDescriptorPool->allocDescriptorSet(*mStaticParamDescriptorSetLayout, imageCount);

            // Create descriptor sets
            for (uint32_t i = 0; i < imageCount; ++i) {
                std::array<WriteDescriptorSet, 1> descriptorWrites = {
                    mCameraUniforms[i].getWriteDescriptor(0, vk::DescriptorType::eUniformBuffer),
                };

                mStaticDescriptorSets[i].updateDescriptor(descriptorWrites);
            }

        }

        void BallShader::buildPropertyBlock() {
            std::vector<MaterialPropertyInfo> properties = {
                MaterialPropertyInfo("mainColor",               MaterialPropertyType::VECTOR,   Vector4f::One),
                MaterialPropertyInfo("rimColor",                MaterialPropertyType::VECTOR,   Vector4f::Zero),
                MaterialPropertyInfo("rimPower",                MaterialPropertyType::FLOAT,      1.0f),
                MaterialPropertyInfo("rimIntensity",            MaterialPropertyType::FLOAT,    1.0f),
            };
            mMaterialPropertySet.insert(properties.begin(), properties.end());

            for (auto i = 0; i < mDefaultMaterialCount; ++i)
                mUnusedId.push_back(i);
        }



        void BallShader::buildPipeline() {
            /*{
                GraphicsPipelineStateDesc desc;

                auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/BallShaderInstance.vert");
                auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/BallShader.frag");
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

                desc.descriptorSetLayouts = { mStaticParamDescriptorSetLayout };
                desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

                desc.pushConstant.emplace_back(vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4));

                mGraphicsPipeline = std::make_shared<GraphicsPipelineState>(mDevice, desc);
            }*/

            // GPU instance version
            {
                GraphicsPipelineStateDesc desc;

                auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/BallShaderInstance.vert");
                auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/BallShader.frag");
                std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
                std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

                desc.meshVertexDecl = std::make_shared<VertexDeclaration>(VertexAttribute::Position | VertexAttribute::Normal | VertexAttribute::UV0);
                std::vector<VertexElement> ves = {
                    VertexElement(1,3,                    0, VertexElementType::Float4, VertexElementSemantic::Color,0,1),
                    VertexElement(1,4,     sizeof(Vector4f), VertexElementType::Float4, VertexElementSemantic::Color,1,1),
                    VertexElement(1,5, 2 * sizeof(Vector4f),VertexElementType::Float4, VertexElementSemantic::Color,2,1),
                    VertexElement(1,6, 3 * sizeof(Vector4f),VertexElementType::Float4, VertexElementSemantic::Color,3,1),
                };
                desc.extraVertexDecl = std::make_shared<VertexDeclaration>(ves);

                desc.gpuProgram.vertex = vertShader;
                desc.gpuProgram.fragment = fragShader;

                desc.cullMode = vk::CullModeFlagBits::eBack;
                desc.frontFace = vk::FrontFace::eCounterClockwise;
                desc.polygonMode = vk::PolygonMode::eFill;

                desc.enableDepthTest = true;
                desc.enableWriteDepth = true;

                desc.descriptorSetLayouts = { mStaticParamDescriptorSetLayout };
                desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

                desc.pushConstant.emplace_back(vk::ShaderStageFlagBits::eFragment, 0, sizeof(BallShaderParameter));

                mInstanceGraphicsPipeline = std::make_shared<GraphicsPipelineState>(mDevice, desc);
            }
        }
    }
}
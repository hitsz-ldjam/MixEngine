#include "MxVkStandardShader.h"
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
        StandardShader::StandardShader(VulkanAPI* _vulkan) :ShaderBase(_vulkan) {
            mDevice = mVulkan->getLogicalDevice();

            auto imageCount = mVulkan->getSwapchain()->imageCount();
            mDynamicUniform.reserve(imageCount);
            // mTestDynamic.reserve(imageCount);
            mCameraUniforms.reserve(imageCount);

            for (size_t i = 0; i < imageCount; ++i) {
                mDynamicUniform.emplace_back(mVulkan->getAllocator(),
                                             sizeof(Uniform::MeshUniform),
                                             120);

                mCameraUniforms.emplace_back(mVulkan->getAllocator(),
                                             vk::BufferUsageFlagBits::eUniformBuffer,
                                             vk::MemoryPropertyFlagBits::eHostVisible |
                                             vk::MemoryPropertyFlagBits::eHostCoherent,
                                             sizeof(Uniform::CameraUniform));
            }

            // buildRenderPass();
            // buildFrameBuffer();
            buildDescriptorSetLayout();
            buildPipeline();
            buildDescriptorSet();
            buildPropertyBlock();
        }

        StandardShader::~StandardShader() {
            mDevice->get().destroyImageView(mDepthStencilView);
        }

        void StandardShader::beginRender(const Camera& _camera) {
            mCurrFrame = mVulkan->getCurrFrame();
            mCurrCmd = &mVulkan->getCurrDrawCmd();
            mCurrVertexInput = nullptr;
            mCurrPipeline = nullptr;

            auto& cmd = mVulkan->getCurrDrawCmd();

            // update Camera
            setCamera(_camera);
        }

        void StandardShader::endRender() {
            // mDynamicUniform[mCurrFrame].reset();
        }

        void StandardShader::setCamera(const Camera& _camera) {
            // update Camera
            Uniform::CameraUniform ubo;
            ubo.position = _camera.transform()->getPosition();
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

        void StandardShader::beginElement(const RenderElement& _element) {
            //Uniform::MeshUniform uniform;
            //uniform.modelMat = _renderer.transform->localToWorldMatrix();
            //mDynamicUniform[mCurrFrame].pushBack(&uniform, sizeof uniform);
            mCurrCmd->get().pushConstants<Matrix4>(mGraphicsPipelineState->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, _element.transform->localToWorldMatrix());
            mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               mGraphicsPipelineState->getPipelineLayout(),
                                               0,
                                               mStaticDescriptorSets[mCurrFrame].get(),
                                               nullptr);
        }

        void StandardShader::endElement() {
            // mDynamicUniform[mCurrFrame].next();
        }

        bool StandardShader::choosePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh) {
            bool depthWrite = _material.getRenderType() != RenderType::Transparent;

            auto newVertexInput = mVulkan->getVertexInputManager().getVertexInput(*_mesh.getVertexDeclaration(), *mGraphicsPipelineState->getVertexDeclaration());
            if (newVertexInput == nullptr) // This mesh is not compatiple with this pipeline
                return false;
            if (newVertexInput != mCurrVertexInput) {
                auto newPipeline = mGraphicsPipelineState->getPipeline(mVulkan->getRenderPass(), 0, newVertexInput, _mesh.getTopology(_submesh), true, depthWrite);
                if (newPipeline != mCurrPipeline) {
                    mCurrCmd->get().bindPipeline(vk::PipelineBindPoint::eGraphics, newPipeline->get());
                    mCurrPipeline = newPipeline;
                }
                mCurrVertexInput = newVertexInput;
            }
            return true;
        }

        void StandardShader::updateMaterial(Material& _material) {
            if (!_material._getChangedList().empty()) {
                std::vector<WriteDescriptorSet> writes;
                for (auto& name : _material._getChangedList()) {
                    if (_material.getTexture(name))
                        writes.push_back(_material.getTexture(name)->getWriteDescriptor(mMaterialNameBindingMap[name], vk::DescriptorType::eCombinedImageSampler));;
                }
                std::swap(mMaterialDescs[0][_material._getMaterialId()], mMaterialDescs[1][_material._getMaterialId()]);
                mMaterialDescs[0][_material._getMaterialId()].updateDescriptor(writes);
            }
            _material._updated();
        }

        void StandardShader::setMaterail(Material& _material) {
            updateMaterial(_material);
            mCurrCmd->get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                               mCurrPipeline->pipelineLayout(), 1,
                                               mMaterialDescs[0][_material._getMaterialId()].get(),
                                               nullptr);
        }

        void StandardShader::render(RenderElement& _element) {
            beginElement(_element);

            choosePipeline(*_element.material, *_element.mesh, _element.submesh);
            setMaterail(*_element.material);
            DrawMesh(*mCurrCmd, *_element.mesh, _element.submesh);

            endElement();
        // Test Gui
        //if (ImGui::GetDrawData()->CmdListsCount > 0) {
        //	_cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, mGuiPipeline->get());
        //	_cmd.get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
        //								  mGuiPipeline->pipelineLayout(),
        //								  0,
        //								  mGuiDescriptorSet,
        //								  nullptr);
        //	_cmd.get().bindVertexBuffers(0, mUi->getVertexBuffer().get(), { 0 });
        //	_cmd.get().bindIndexBuffer(mUi->getIndexBuffer().get(), 0, mUi->getIndexType());

        //	_cmd.get().pushConstants(mGuiPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0,
        //							 sizeof(Math::Vector2f), mUi->getScale().linear);
        //	_cmd.get().pushConstants(mGuiPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex,
        //							 sizeof(Math::Vector2f), sizeof(Math::Vector2f), mUi->getTranslate().linear);

        //	auto drawData = mUi->getDrawData();

        //	int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
        //	int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

        //	vk::Viewport viewport;
        //	viewport.x = 0;
        //	viewport.y = 0;
        //	viewport.width = static_cast<float>(fbWidth);
        //	viewport.height = static_cast<float>(fbHeight);
        //	viewport.minDepth = 0.0f;
        //	viewport.maxDepth = 1.0f;
        //	_cmd.get().setViewport(0, viewport);

        //	Math::Vector2f clipOff{ drawData->DisplayPos.x, drawData->DisplayPos.y };
        //	// (0,0) unless using multi-viewports
        //	Math::Vector2f clipScale{ drawData->FramebufferScale.x, drawData->FramebufferScale.y };
        //	// (1,1) unless using retina display which are often (2,2)

        //	// Render command lists
        //	// (Because we merged all buffers into a single one, we maintain our own offset into them)
        //	int vtxOffset = 0;
        //	int idxOffset = 0;
        //	for (int n = 0; n < drawData->CmdListsCount; n++) {
        //		const ImDrawList* cmdList = drawData->CmdLists[n];
        //		for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++) {
        //			const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
        //			// Project scissor/clipping rectangles into framebuffer space
        //			Math::Vector4f clipRect;
        //			clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
        //			clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
        //			clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
        //			clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

        //			if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f) {
        //				// Negative offsets are illegal for vkCmdSetScissor
        //				if (clipRect.x < 0.0f)
        //					clipRect.x = 0.0f;
        //				if (clipRect.y < 0.0f)
        //					clipRect.y = 0.0f;

        //				// Apply scissor/clipping rectangle
        //				vk::Rect2D scissor;
        //				scissor.offset.x = static_cast<int32_t>(clipRect.x);
        //				scissor.offset.y = static_cast<int32_t>(clipRect.y);
        //				scissor.extent.width = static_cast<uint32_t>(clipRect.z - clipRect.x);
        //				scissor.extent.height = static_cast<uint32_t>(clipRect.w - clipRect.y);
        //				_cmd.get().setScissor(0, 1, &scissor);
        //				// Draw
        //				_cmd.get().drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + idxOffset,
        //									   pcmd->VtxOffset + vtxOffset, 0);
        //			}
        //		}
        //		idxOffset += cmdList->IdxBuffer.Size;
        //		vtxOffset += cmdList->VtxBuffer.Size;
        //	}
        //}
        }

        void StandardShader::update(const Shader& _shader) {
        }

        uint32_t StandardShader::newMaterial() {
            uint32_t result = mUnusedId.back();
            mUnusedId.pop_back();
            return result;
        }

        void StandardShader::deleteMaterial(uint32_t _id) {
            mUnusedId.push_back(_id);
        }

        void StandardShader::buildDescriptorSet() {
            auto imageCount = mVulkan->getSwapchain()->imageCount();

            mDescriptorPool = std::make_shared<DescriptorPool>(mDevice);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, imageCount);
            // mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBufferDynamic, imageCount);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eCombinedImageSampler, 1 * mDefaultMaterialCount * imageCount);
            mDescriptorPool->create((mDefaultMaterialCount + 1)*imageCount);


            mStaticDescriptorSets = mDescriptorPool->allocDescriptorSet(*mStaticParamDescriptorSetLayout, imageCount);

            // Create descriptor sets
            for (uint32_t i = 0; i < imageCount; ++i) {
                std::array<WriteDescriptorSet, 1> descriptorWrites = {
                    mCameraUniforms[i].getWriteDescriptor(0, vk::DescriptorType::eUniformBuffer),
                    // mDynamicUniform[i].getWriteDescriptor(1)
                };

                mStaticDescriptorSets[i].updateDescriptor(descriptorWrites);
            }

            mMaterialDescs.resize(imageCount);
            for (uint32_t i = 0; i < imageCount; ++i)
                mMaterialDescs[i] = mDescriptorPool->allocDescriptorSet(*mDynamicPamramDescriptorSetLayout, mDefaultMaterialCount);


            /*mGuiDescriptorSet = mVulkan->getDescriptorPool()->allocDescriptorSet(*mGuiPipeline->descriptorSetLayouts()[0].get());

            mUi = Ui::Get();
            vk::DescriptorImageInfo info;
            info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            info.sampler = mUi->getFontTexture().getSampler();
            info.imageView = mUi->getFontTexture().getImageView();

            vk::WriteDescriptorSet write{ mGuiDescriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &info };
            mDevice->get().updateDescriptorSets(write, nullptr);*/
        }

        void StandardShader::buildPropertyBlock() {
            mMaterialNameBindingMap["diffuseTex"] = 0;

            mMaterialPropertySet.insert(MaterialPropertyInfo("diffuseTex", MaterialPropertyType::TEX_2D, std::shared_ptr<Texture>()));
            for (auto i = 0; i < mDefaultMaterialCount; ++i)
                mUnusedId.push_back(i);
        }

        //void StandardShader::buildRenderPass() {
        //    mDepthStencil = Image::CreateDepthStencil(mVulkan->getAllocator(),
        //                                              mSwapchain->extent(),
        //                                              vk::SampleCountFlagBits::e1);
        //    mDepthStencilView = Image::CreateVkImageView2D(mDevice->get(),
        //                                                   mDepthStencil->get(),
        //                                                   mDepthStencil->format(),
        //                                                   vk::ImageAspectFlagBits::eDepth |
        //                                                   vk::ImageAspectFlagBits::eStencil);

        //    // RenderPass
        //    mRenderPass = std::make_shared<RenderPass>(mVulkan->getLogicalDevice());

        //    mRenderPass->addAttachment({
        //        {0, Attachment::Type::PRESENT, mSwapchain->surfaceFormat().format},
        //        {1, Attachment::Type::DEPTH_STENCIL, mDepthStencil->format()}
        //                               });

        //    Subpass subpass(0);
        //    subpass.addRef({
        //        {AttachmentRef::Type::COLOR, 0},
        //        {AttachmentRef::Type::DEPTH_STENCIL, 1}
        //                   });

        //    mRenderPass->addSubpass(subpass);

        //    mRenderPass->addDependency({
        //        VK_SUBPASS_EXTERNAL,
        //        0,
        //        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        //        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        //        vk::AccessFlags(),
        //        vk::AccessFlagBits::eColorAttachmentRead |
        //        vk::AccessFlagBits::eColorAttachmentWrite
        //                               });
        //    mRenderPass->create();
        //}

        /*void StandardShader::buildFrameBuffer() {
            for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
                mFrameBuffers.emplace_back(mRenderPass, mVulkan->getSwapchain()->extent());
                mFrameBuffers.back().addAttachments({ mSwapchain->getImageViews()[i], mDepthStencilView });
                mFrameBuffers.back().create();
            }
        }*/

        void StandardShader::buildDescriptorSetLayout() {
            mStaticParamDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mStaticParamDescriptorSetLayout->setBindings(
                {
                    {0,vk::DescriptorType::eUniformBuffer,1,vk::ShaderStageFlagBits::eVertex}
                }
            );
            mStaticParamDescriptorSetLayout->create();

            mDynamicPamramDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mDynamicPamramDescriptorSetLayout->setBindings(
                {
                    {0,vk::DescriptorType::eCombinedImageSampler,1,vk::ShaderStageFlagBits::eFragment}
                }
            );
            mDynamicPamramDescriptorSetLayout->create();

        }

        void StandardShader::buildPipeline() {
            GraphicsPipelineStateDesc desc;

            auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/vShader.vert");
            auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/fShader.frag");
            std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
            std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

            desc.vertexDecl = std::make_shared<VertexDeclaration>(VertexAttribute::Position | VertexAttribute::Normal | VertexAttribute::UV0);

            desc.gpuProgram.vertex = vertShader;
            desc.gpuProgram.fragment = fragShader;

            desc.cullMode = vk::CullModeFlagBits::eBack;
            desc.frontFace = vk::FrontFace::eCounterClockwise;
            desc.polygonMode = vk::PolygonMode::eFill;

            desc.enableDepthTest = true;
            desc.enableWriteDepth = true;

            desc.descriptorSetLayouts = { mStaticParamDescriptorSetLayout,mDynamicPamramDescriptorSetLayout };
            desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

            desc.pushConstant.push_back(vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4)));

            mGraphicsPipelineState = std::make_shared<GraphicsPipelineState>(mDevice, desc);
            /*std::ifstream inFile;
            inFile.open("TestResources/pipeline/pipeline.json");
            nlohmann::json json = nlohmann::json::parse(inFile);
            mPipeline = PipelineFactory::CreatePipelineFromJson(mRenderPass, 0, json, viewport, scissor);*/

            //inFile.close();
            //// Test Gui
            //inFile.open("TestResources/pipeline/gui_pipeline.json");
            //json = nlohmann::json::parse(inFile);
            //mGuiPipeline = PipelineFactory::CreatePipelineFromJson(mRenderPass, 0, json, viewport, scissor);
        }
    }
}

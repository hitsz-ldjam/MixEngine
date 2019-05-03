#include "MxVkGraphics.h"
#include "MxVkUniform.h"
#include "../Mx/MxRender.h"

namespace Mix {
    namespace Graphics {
        void Vulkan::Init() {
            mCore = std::make_shared<Core>();
            mDebug = std::make_shared<Debug>();
            mSwapchain = std::make_shared<Swapchain>();

            mShaderMgr = std::make_shared<ShaderMgr>();
            mPipelineMgr = std::make_shared<PipelineMgr>();

            mRenderPass = std::make_shared<RenderPass>();
            mDescriptorPool = std::make_shared<DescriptorPool>();
            mCommandMgr = std::make_shared<CommandMgr>();

            mImageMgr = std::make_shared<ImageMgr>();
            mMeshMgr = std::make_shared<gltf::MeshMgr>();

        }

        void Vulkan::Build() {
            BuildCore();
            BuildDebugUtils();
            BuildCommandMgr();
            mDescriptorPool->Init(mCore);
            mAllocator = std::make_shared<DeviceAllocator>();
            mAllocator->Init(mCore);
            BuildSwapchain();
            BuildDepthStencil();
            BuildRenderPass();
            BuildDescriptorSetLayout();
            BuildShaders();
            BuildPipeline();
            BuildFrameBuffers();

            LoadResource();

            BuildUniformBuffers();
            BuildDescriptorSets();
            BuildCommandBuffers();
        }

        void Vulkan::Update(const float _deltaTime) {
            UpdateUniformBuffer(_deltaTime);
            UpdateCmdBuffer(_deltaTime);

            mSwapchain->present(mCommandBuffers[mSwapchain->currentFrame()]);
        }

        void Vulkan::UpdateCmdBuffer(float _deltaTime) {
            const auto currFrame = mSwapchain->currentFrame();

            const vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
            mCommandBuffers[currFrame].begin(beginInfo);

            std::vector<vk::ClearValue> clearValues(2);
            clearValues[0].color = std::array<float, 4>{0.0f, 0.75f, 1.0f, 1.0f};
            clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

            //begin render pass
            mRenderPass->beginRenderPass(mCommandBuffers[currFrame], mFramebuffers[currFrame]->get(), clearValues,
                                         mSwapchain->extent());

            mCommandBuffers[currFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, mPipelineMgr->getPipeline("pipeline").get());

            mCommandBuffers[currFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                          mPipelineMgr->getPipeline("pipeline").pipelineLayout(),
                                                          0,
                                                          mDescriptorSets[currFrame],
                                                          nullptr);

            std::vector<MeshRenderer*> renderers = Object::FindObjectsOfType<MeshRenderer>();
            for (MeshRenderer* renderer : renderers) {
                RenderInfo info = renderer->getRenderInfo();

                mCommandBuffers[currFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                              mPipelineMgr->getPipeline("pipeline").pipelineLayout(),
                                                              1,
                                                              renderer->descriptorSet(currFrame),
                                                              nullptr);

                auto mesh = mMeshMgr->getMesh(info.meshRef.model, info.meshRef.mesh);

                mCommandBuffers[currFrame].bindVertexBuffers(0,
                                                             mesh.vertexBuffer,
                                                             { 0 });

                mCommandBuffers[currFrame].bindIndexBuffer(mesh.indexBuffer, 0, vk::IndexType::eUint32);

                for (auto& subMesh : mesh.meshInfo.submeshes) {


                    mCommandBuffers[currFrame].drawIndexed(subMesh.indexCount,
                                                           1,
                                                           subMesh.firstIndex,
                                                           subMesh.firstVertex,
                                                           0);
                }
            }
            //end render pass
            mRenderPass->endRenderPass(mCommandBuffers[currFrame]);

            //end command buffer
            mCommandBuffers[currFrame].end();
        }

        void Vulkan::Vulkan::UpdateUniformBuffer(float _deltaTime) {
            auto objects = Object::FindObjectsOfType<MeshRenderer>();
            for (auto obj : objects) {
                auto buffer = obj->uniformBuffers(mSwapchain->currentFrame());
                auto& uniform = obj->uniform();
                buffer->CopyTo(&uniform, sizeof(uniform));
            }

            CameraUniform ubo;
            ubo.position = glm::vec4(0.0f, 0.0f, 3.0f, 1.0f);
            const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
            ubo.forward = glm::vec4(glm::normalize(target - glm::vec3(ubo.position)), 0.0f);
            ubo.viewMat = glm::lookAt(glm::vec3(ubo.position),
                                      target,
                                      glm::vec3(0.0f, 1.0f, 0.0f));

            ubo.projMat = glm::perspective(glm::radians(45.0f),
                                           float(WIN_WIDTH) / WIN_HEIGHT,
                                           0.1f, 1000.0f);
            ubo.projMat[1][1] *= -1.0f;

            uniforms[mSwapchain->currentFrame()]->CopyTo(&ubo, sizeof(ubo));
        }

        void Vulkan::Destroy() {
            mCore->GetDevice().waitIdle();

            mCore->GetDevice().destroyImageView(mDepthStencilView);
            mCore->GetDevice().destroyImage(mDepthStencil.image);
            mCore->GetDevice().freeMemory(mDepthStencil.memory);

            mCore->GetDevice().destroyImageView(texImageView);
            mCore->GetDevice().destroySampler(sampler);

            for (auto& buffer : uniforms) {
                delete buffer;
            }

            for (auto& frameBuffer : mFramebuffers)
                delete frameBuffer;
        }



        void Vulkan::BuildCore() {
            mCore->SetAppInfo("Demo", Mix::Version::makeVersion(0, 0, 1));
            mCore->SetDebugMode(true);
            mCore->SetTargetWindow(mWindow);
            mCore->SetQueueFlags();
            mCore->CreateInstance();
            mCore->PickPhysicalDevice();
            mCore->CreateLogicalDevice();
            mCore->EndInit();
        }

        void Vulkan::BuildDebugUtils() {
            mDebug->Init(mCore);
            mDebug->setDefaultCallback();
        }

        void Vulkan::BuildSwapchain() {
            mSwapchain->Init(mCore);
            mSwapchain->setImageCount(2);
            mSwapchain->create(mSwapchain->supportedFormat(),
                               { vk::PresentModeKHR::eFifo },
                               vk::Extent2D(640, 480));
        }

        void Vulkan::BuildDepthStencil() {
            mDepthStencil = Tools::createDepthStencil(*mCore,
                                                      mSwapchain->extent(),
                                                      mSettings.sampleCount);

            mDepthStencilView = Tools::createImageView2D(mCore->GetDevice(),
                                                         mDepthStencil.image,
                                                         mDepthStencil.format,
                                                         vk::ImageAspectFlagBits::eDepth |
                                                         vk::ImageAspectFlagBits::eStencil);
        }

        void Vulkan::BuildRenderPass() {
            mRenderPass->Init(mCore);
            const auto presentAttach = mRenderPass->addColorAttach(mSwapchain->surfaceFormat().format,
                                                                   mSettings.sampleCount,
                                                                   vk::AttachmentLoadOp::eClear,
                                                                   vk::AttachmentStoreOp::eStore,
                                                                   vk::ImageLayout::eUndefined,
                                                                   vk::ImageLayout::ePresentSrcKHR);

            const auto presentAttahRef = mRenderPass->addColorAttachRef(presentAttach);

            const auto depthAttach = mRenderPass->addDepthStencilAttach(mDepthStencil.format, mSettings.sampleCount);

            const auto depthAttachRef = mRenderPass->addDepthStencilAttachRef(depthAttach);

            const auto subpass = mRenderPass->addSubpass();
            mRenderPass->addSubpassColorRef(subpass, presentAttahRef);
            mRenderPass->addSubpassDepthStencilRef(subpass, depthAttachRef);

            mRenderPass->addDependency(VK_SUBPASS_EXTERNAL,
                                       subpass,
                                       vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                       vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                       vk::AccessFlags(),
                                       vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

            mRenderPass->create();
        }

        void Vulkan::BuildDescriptorSetLayout() {
            auto cameraLayout = std::make_shared<DescriptorSetLayout>();
            cameraLayout->Init(mCore);
            cameraLayout->addBindings(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
            cameraLayout->create();

            auto objLayout = std::make_shared<DescriptorSetLayout>();
            objLayout->Init(mCore);
            objLayout->addBindings(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
            objLayout->addBindings(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);
            objLayout->create();

            mDescriptorSetLayout["Camera"] = cameraLayout;
            mDescriptorSetLayout["Object"] = objLayout;
        }

        void Vulkan::BuildShaders() {
            mShaderMgr->Init(mCore);
            std::ifstream inFile("Shaders/vShader.vert.spv", std::ios_base::ate | std::ios_base::binary);
            size_t fileSize = static_cast<uint32_t>(inFile.tellg());
            std::vector<char> shaderCode(fileSize);
            inFile.seekg(std::ios_base::beg);
            inFile.read(shaderCode.data(), fileSize);
            inFile.close();
            mShaderMgr->createShader("vShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eVertex);

            inFile.open("Shaders/fShader.frag.spv", std::ios_base::ate | std::ios_base::binary);;
            fileSize = static_cast<uint32_t>(inFile.tellg());
            shaderCode.resize(fileSize);
            inFile.seekg(std::ios_base::beg);
            inFile.read(shaderCode.data(), fileSize);
            inFile.close();
            mShaderMgr->createShader("fShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eFragment);
        }

        void Vulkan::BuildPipeline() {
            mPipelineMgr->Init(mCore);
            auto& pipeline = mPipelineMgr->createPipeline("pipeline", mRenderPass->get(), 0);

            auto& vertexShader = mShaderMgr->getModule("vShader");
            auto& fragShader = mShaderMgr->getModule("fShader");

            pipeline.addShader(vertexShader.stage, vertexShader.module);
            pipeline.addShader(fragShader.stage, fragShader.module);

            pipeline.setVertexInput(Vertex::GetBindingDescrip(), Vertex::GetAttributeDescrip());
            pipeline.setInputAssembly();

            vk::Viewport viewPort;
            viewPort.x = 0;
            viewPort.y = 0;
            viewPort.width = static_cast<float>(WIN_WIDTH);
            viewPort.height = static_cast<float>(WIN_HEIGHT);
            viewPort.minDepth = 0.0f;
            viewPort.maxDepth = 1.0f;
            pipeline.addViewport(viewPort);

            vk::Rect2D scissor;
            scissor.extent = mSwapchain->extent();
            scissor.offset = vk::Offset2D(0, 0);
            pipeline.addScissor(scissor);

            pipeline.setRasterization(vk::PolygonMode::eFill,
                                      vk::CullModeFlagBits::eBack,
                                      vk::FrontFace::eCounterClockwise);
            pipeline.setDepthBias(false);

            pipeline.setMultiSample(mSettings.sampleCount);

            pipeline.setDepthTest(true);
            pipeline.setDepthBoundsTest(false);
            pipeline.setStencilTest(false);

            pipeline.addDefaultBlendAttachments();

            pipeline.addDescriptorSetLayout(mDescriptorSetLayout["Camera"]->get());
            pipeline.addDescriptorSetLayout(mDescriptorSetLayout["Object"]->get());

            pipeline.create();
        }

        void Vulkan::BuildCommandMgr() {
            mCommandMgr->Init(mCore);
            mCommandMgr->create(vk::QueueFlagBits::eGraphics, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        }

        void Vulkan::BuildFrameBuffers() {


            mFramebuffers.resize(mSwapchain->imageCount());
            for (uint32_t i = 0; i < mFramebuffers.size(); ++i) {
                mFramebuffers[i] = new Framebuffer();

                mFramebuffers[i]->Init(mCore);
                mFramebuffers[i]->setExtent(mSwapchain->extent());
                mFramebuffers[i]->setLayers(1);
                mFramebuffers[i]->setTargetRenderPass(mRenderPass->get());
                mFramebuffers[i]->addAttachments({ mSwapchain->imageViews()[i],mDepthStencilView });
                mFramebuffers[i]->create();
            }
        }

        void Vulkan::Vulkan::BuildUniformBuffers() {
            uniforms.resize(mSwapchain->imageCount());

            for (size_t i = 0; i < uniforms.size(); ++i) {
                uniforms[i] = Buffer::CreateBuffer(mCore,
                                                   mAllocator,
                                                   vk::BufferUsageFlagBits::eUniformBuffer,
                                                   vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent,
                                                   sizeof(CameraUniform));
            }
        }

        void Vulkan::Vulkan::BuildCommandBuffers() {
            mCommandBuffers = mCommandMgr->allocCommandBuffers(mSwapchain->imageCount(),
                                                               vk::CommandBufferLevel::ePrimary);
        }

        void Vulkan::Vulkan::BuildDescriptorSets() {
            //create descriptor pool
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, mSwapchain->imageCount() * 5);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eCombinedImageSampler, mSwapchain->imageCount() * 5);
            mDescriptorPool->create(mSwapchain->imageCount() * 5);

            // test Allocate camera descriptor set
            mDescriptorSets = mDescriptorPool->allocDescriptorSet(mDescriptorSetLayout["Camera"]->get(), mSwapchain->imageCount());

            // update descriptor sets
            for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
                std::array<vk::WriteDescriptorSet, 1> descriptorWrites = {};
                descriptorWrites[0].dstSet = mDescriptorSets[i]; //descriptor which will be write in
                descriptorWrites[0].dstBinding = 0; //destination binding
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer; //the type of the descriptor that will be wirte in
                descriptorWrites[0].descriptorCount = 1; //descriptor count
                descriptorWrites[0].pBufferInfo = &uniforms[i]->descriptor; //descriptorBufferInfo
                descriptorWrites[0].pImageInfo = nullptr;
                descriptorWrites[0].pTexelBufferView = nullptr;

                mCore->GetDevice().updateDescriptorSets(descriptorWrites, nullptr);
            }
        }

        void Vulkan::Vulkan::LoadResource() {
            mImageMgr->init(mCore, mAllocator);
            mMeshMgr->init(mCore, mAllocator);

            const gli::texture2d texture(gli::load("Texture/1.DDS"));
            auto cmd = mCommandMgr->allocCommandBuffer();
            mImageMgr->beginLoad(cmd);
            mImageMgr->loadTexture("front", texture);
            mImageMgr->endLoad();
            mCommandMgr->freeCommandBuffers(cmd);

            vk::ImageViewCreateInfo viewInfo;
            viewInfo.viewType = vk::ImageViewType::e2D;
            viewInfo.format = mImageMgr->getImage("front").format;
            viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.image = mImageMgr->getImage("front").image;

            texImageView = mCore->GetDevice().createImageView(viewInfo);

            vk::SamplerCreateInfo samplerInfo;
            samplerInfo.magFilter = vk::Filter::eLinear;
            samplerInfo.minFilter = vk::Filter::eLinear;
            samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
            samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
            samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
            samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;

            sampler = mCore->GetDevice().createSampler(samplerInfo);
        }
    }
}

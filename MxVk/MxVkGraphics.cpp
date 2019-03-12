#include "MxVkGraphics.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Graphics, GraphicsBase);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Graphics);

        void Graphics::init() {
            mCore = new Core();
            mDebug = new Debug();
            mSwapchain = new Swapchain();

            mShaderMgr = new ShaderMgr();
            mPipelineMgr = new PipelineMgr();
            mSyncObjMgr = new SyncObjectMgr();

            mRenderPass = new RenderPass();
            mDescriptorPool = new DescriptorPool();
            mDescriptorSetLayout = new DescriptorSetLayout();
            mCommandMgr = new CommandMgr();
        }

        void Graphics::build() {
            buildCore();
            mSyncObjMgr->init(mCore);
            mDescriptorPool->init(mCore);
            mDescriptorSetLayout->init(mCore);
            buildDebugUtils();
            buildSwapchain();
            buildDepthStencil();
            buildRenderPass();
            buildDescriptorSetLayout();
            buildShaders();
            buildPipeline();
            buildCommandMgr();
            buildFrameBuffers();

            buildVertexBuffers();
            buildUniformBuffers();
            buildDescriptorSets();
            buildCommandBuffers();
        }

        void Graphics::update(float deltaTime) {
            Uniform ubo = {};
            ubo.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(0.0f, 0.0f, 1.0f));

            ubo.proj = glm::perspective(glm::radians(45.0f),
                                        float(WIN_WIDTH) / WIN_HEIGHT,
                                        0.1f, 100.0f);
            ubo.proj[1][1] *= -1.0f;

            static float angle = 0.0f;
            ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians((angle += deltaTime * 10.0f)), glm::vec3(0.0f, 0.0f, 1.0f));

            uniforms[mSwapchain->currentFrame()]->map();
            uniforms[mSwapchain->currentFrame()]->copyTo(&ubo, sizeof(ubo));
            uniforms[mSwapchain->currentFrame()]->unmap();

            mSwapchain->present(mCommandBuffers[mSwapchain->currentFrame()]);
        }

        void Graphics::destroy() {
            mCore->device().waitIdle();

            delete mCommandMgr;

            mCore->device().destroyImageView(mDepthStencilView);
            mCore->device().destroyImage(mDepthStencil.image);
            mCore->device().freeMemory(mDepthStencil.memory);

            for (auto& buffer : uniforms) {
                delete buffer;
            }

            delete box;

            for (auto& framebuffer : mFramebuffers)
                delete framebuffer;

            delete mShaderMgr;
            delete mSwapchain;
            delete mDescriptorSetLayout;
            delete mDescriptorPool;
            delete mPipelineMgr;
            delete mRenderPass;

            delete mSyncObjMgr;
            delete mDebug;
            delete mCore;
        }

        void Graphics::buildCore() {
            mCore->setAppInfo("Demo", Mix::Version::makeVersion(0, 0, 1));
            mCore->setDebugMode(true);
            mCore->setTargetWindow(mWindow);
            mCore->setQueueFlags();
            mCore->createInstance();
            mCore->pickPhysicalDevice();
            mCore->pickPhysicalDevice();
            mCore->createLogicalDevice();
            mCore->endInit();
        }

        void Graphics::buildDebugUtils() {
            mDebug->init(mCore);
            mDebug->setDefaultCallback();
        }

        void Graphics::buildSwapchain() {
            // todo
            mSwapchain->init(mCore);
            mSwapchain->initSyncObj(*mSyncObjMgr);
            mSwapchain->create(mSwapchain->supportedFormat(),
                               vk::PresentModeKHR::eFifo,
                               vk::Extent2D(640, 480));
        }

        void Graphics::buildDepthStencil() {
            mDepthStencil = Tools::createDepthStencil(*mCore,
                                                      vk::Format::eD24UnormS8Uint,
                                                      mSwapchain->extent(),
                                                      mSettings.sampleCount);

            mDepthStencilView = Tools::createImageView2D(mCore->device(),
                                                         mDepthStencil.image,
                                                         mDepthStencil.format,
                                                         vk::ImageAspectFlagBits::eDepth |
                                                         vk::ImageAspectFlagBits::eStencil);
        }

        void Graphics::buildRenderPass() {
            mRenderPass->init(mCore);
            auto presentAttach = mRenderPass->addColorAttach(mSwapchain->surfaceFormat().format,
                                                             mSettings.sampleCount,
                                                             vk::AttachmentLoadOp::eClear,
                                                             vk::AttachmentStoreOp::eStore,
                                                             vk::ImageLayout::eUndefined,
                                                             vk::ImageLayout::ePresentSrcKHR);

            auto presentAttahRef = mRenderPass->addColorAttachRef(presentAttach);

            auto depthAttach = mRenderPass->addDepthStencilAttach(mDepthStencil.format, mSettings.sampleCount);

            auto depthAttachRef = mRenderPass->addDepthStencilAttachRef(depthAttach);

            auto subpass = mRenderPass->addSubpass();
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

        void Graphics::buildDescriptorSetLayout() {
            mDescriptorSetLayout->addBindings(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
            mDescriptorSetLayout->create();
        }

        void Graphics::buildShaders() {
            mShaderMgr->init(mCore);
            std::ifstream inFile("Shaders/vShader.spv", std::ios_base::ate | std::ios_base::binary);
            size_t fileSize = static_cast<uint32_t>(inFile.tellg());
            std::vector<char> shaderCode(fileSize);
            inFile.seekg(std::ios_base::beg);
            inFile.read(shaderCode.data(), fileSize);
            inFile.close();
            mShaderMgr->createShader("vShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eVertex);

            inFile.open("Shaders/fShader.spv", std::ios_base::ate | std::ios_base::binary);;
            fileSize = static_cast<uint32_t>(inFile.tellg());
            shaderCode.resize(fileSize);
            inFile.seekg(std::ios_base::beg);
            inFile.read(shaderCode.data(), fileSize);
            inFile.close();
            mShaderMgr->createShader("fShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eFragment);
        }

        void Graphics::buildPipeline() {
            mPipelineMgr->init(mCore);
            auto& pipeline = mPipelineMgr->createPipeline("pipeline", mRenderPass->get(), 0);

            auto& vertexShader = mShaderMgr->getModule("vShader");
            auto& fragShader = mShaderMgr->getModule("fShader");

            pipeline.addShader(vertexShader.stage, vertexShader.module);
            pipeline.addShader(fragShader.stage, fragShader.module);

            pipeline.setVertexInput(Vertex::getBindingDescrip(), Vertex::getAttributeDescrip());
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
            scissor.offset = { 0,0 };
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

            pipeline.addDescriptorSetLayout(mDescriptorSetLayout->get());

            pipeline.create();
        }

        void Graphics::buildCommandMgr() {
            mCommandMgr->init(mCore);
            mCommandMgr->create(vk::QueueFlagBits::eGraphics);
        }

        void Graphics::buildFrameBuffers() {


            mFramebuffers.resize(mSwapchain->imageCount());
            for (uint32_t i = 0; i < mFramebuffers.size(); ++i) {
                mFramebuffers[i] = new Framebuffer();

                mFramebuffers[i]->init(mCore);
                mFramebuffers[i]->setExtent(mSwapchain->extent());
                mFramebuffers[i]->setLayers(1);
                mFramebuffers[i]->setTargetRenderPass(mRenderPass->get());
                mFramebuffers[i]->addAttachments({ mSwapchain->imageViews()[i],mDepthStencilView });
                mFramebuffers[i]->create();
            }
        }

        void Graphics::buildVertexBuffers() {
            vk::DeviceSize bufferSize = sizeof(Vertex)*vertices.size();

            box = Buffer::createBuffer(mCore,
                                       vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                                       vk::MemoryPropertyFlagBits::eDeviceLocal,
                                       bufferSize);

            Buffer::copyToDeviceBuffer(mCore,
                                       mCommandMgr,
                                       box,
                                       vertices.data());
        }

        void Graphics::Graphics::buildUniformBuffers() {
            uniforms.resize(mSwapchain->imageCount());

            for (size_t i = 0; i < uniforms.size(); ++i) {
                uniforms[i] = Buffer::createBuffer(mCore,
                                                   vk::BufferUsageFlagBits::eUniformBuffer,
                                                   vk::MemoryPropertyFlagBits::eHostVisible |
                                                   vk::MemoryPropertyFlagBits::eHostCoherent,
                                                   sizeof(Uniform));
            }
        }

        void Graphics::Graphics::buildCommandBuffers() {
            mCommandBuffers = mCommandMgr->allocCommandBuffers(vk::CommandBufferLevel::ePrimary,
                                                               mSwapchain->imageCount());

            for (size_t i = 0; i < mCommandBuffers.size(); ++i) {
                vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

                mCommandBuffers[i].begin(beginInfo);

                std::vector<vk::ClearValue> clearValues(2);
                clearValues[0].color = std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f};
                clearValues[1].depthStencil = { 1.0f,0 };

                //begin render pass
                mRenderPass->beginRenderPass(mCommandBuffers[i], mFramebuffers[i]->get(), clearValues,
                                             mSwapchain->extent());

                mCommandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, mPipelineMgr->getPipeline("pipeline").get());

                mCommandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                                      mPipelineMgr->getPipeline("pipeline").pipelineLayout(),
                                                      0,
                                                      { mDescriptorSets[i] },
                                                      nullptr);

                mCommandBuffers[i].bindVertexBuffers(0,
                                                     box->buffer,
                                                     { 0 });

                mCommandBuffers[i].draw(static_cast<uint32_t>(vertices.size()),
                                        1,
                                        0,
                                        0);

                //end render pass
                mRenderPass->endRenderPass(mCommandBuffers[i]);

                    //end command buffer
                mCommandBuffers[i].end();
            }
        }

        void Graphics::Graphics::buildDescriptorSets() {
            //create descriptor pool
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, mSwapchain->imageCount());
            mDescriptorPool->create(mSwapchain->imageCount());

            //allocate descriptor sets
            mDescriptorSets = mDescriptorPool->allocDescriptorSet(*mDescriptorSetLayout, mSwapchain->imageCount());



            //update descriptor sets
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

                mCore->device().updateDescriptorSets(descriptorWrites, nullptr);
            }
        }
    }
}

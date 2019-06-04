#include "MxVkGraphics.h"
#include "Buffers/MxVkUniform.h"

#include "../Object/MxObject.h"

#include <fstream>
#include "../GameObject/MxGameObject.h"
#include "../Component/MeshFilter/MxMeshFilter.h"

#include <iostream>

namespace Mix {
	namespace Graphics {
		MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(Vulkan, Object);

		void Vulkan::init() {
			mCore = std::make_shared<Core>();
			mDebug = std::make_shared<DebugUtils>();
			mSwapchain = std::make_shared<Swapchain>();

			mShaderMgr = std::make_shared<ShaderMgr>();
			mPipelineMgr = std::make_shared<PipelineMgr>();

			mRenderPass = std::make_shared<RenderPass>();
			mDescriptorPool = std::make_shared<DescriptorPool>();
			mCommandMgr = std::make_shared<CommandMgr>();

			mImageMgr = std::make_shared<ImageMgr>();
			// mMeshMgr = std::make_shared<gltf::MeshMgr>();

		}

		void Vulkan::build() {
			buildCore();
			buildDebugUtils();
			buildCommandMgr();
			mDescriptorPool->init(mCore);
			mAllocator = std::make_shared<DeviceAllocator>();
			mAllocator->init(mCore);
			buildSwapchain();
			buildDepthStencil();
			buildRenderPass();
			buildDescriptorSetLayout();
			buildShaders();
			buildPipeline();
			buildFrameBuffers();

			loadResource();

			buildUniformBuffers();
			buildDescriptorSets();
			buildCommandBuffers();
		}

		void Vulkan::update() {
			updateUniformBuffer();
			updateCmdBuffer();

			mSwapchain->present(mCommandBuffers[mSwapchain->currentFrame()]);
		}

		void Vulkan::updateCmdBuffer() {
			const auto currFrame = mSwapchain->currentFrame();

			const vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
			mCommandBuffers[currFrame].begin(beginInfo);

			std::vector<vk::ClearValue> clearValues(2);
			clearValues[0].color = std::array<float, 4>{0.0f, 0.75f, 1.0f, 1.0f};
			clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

			//begin render pass
			mRenderPass->beginRenderPass(mCommandBuffers[currFrame], mFramebuffers[currFrame]->get(), clearValues,
										 mSwapchain->extent());

			// todo complete render loop
			auto& cmd = mCommandBuffers[currFrame];

			cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
							 mPipelineMgr->getPipeline("pipeline").get());

			auto filters = Object::FindObjectsOfType<MeshFilter>();
			for (auto filter : filters) {
				auto mesh = filter->getMesh();
				auto& trans = filter->getGameObject()->transform();

				Uniform::MeshUniform uniform;
				uniform.modelMat = trans.localToWorldMatrix();
				uniform.normMat = uniform.modelMat.transpose().inverse();

				dynamicUniformBuffers[currFrame]->pushBack(&uniform, sizeof(uniform));
				cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
									   mPipelineMgr->getPipeline("pipeline").pipelineLayout(),
									   0, mDescriptorSets[currFrame],
									   dynamicUniformBuffers[currFrame]->getAlign());
				dynamicUniformBuffers[currFrame]->next();


				cmd.bindVertexBuffers(0, mesh->getVertexBuffer()->buffer, { 0 });
				cmd.bindIndexBuffer(mesh->getIndexBUffer()->buffer, 0, vk::IndexType::eUint32);
				for (auto& submesh : mesh->getSubmesh()) {
					cmd.drawIndexed(submesh.indexCount,
									1,
									submesh.firstIndex,
									submesh.firstVertex,
									0);
				}
			}


			//end render pass
			mRenderPass->endRenderPass(mCommandBuffers[currFrame]);

			//end command buffer
			mCommandBuffers[currFrame].end();
			dynamicUniformBuffers[currFrame]->reset();
		}

		void Vulkan::updateUniformBuffer() {
			// todo
			auto camera = GameObject::Find("Camera");
			auto& transform = camera->transform();
			Uniform::CameraUniform ubo;

			ubo.position = transform.getPosition();
			ubo.forward = transform.forward();
			auto target = ubo.position + ubo.forward;

			ubo.viewMat = Math::Matrix4::ViewMatrix(ubo.position, target, Math::Vector3f::Up);

			ubo.projMat = Math::Matrix4::Perspective(Math::Radians(45.0f),
													 float(mWindow->drawableSize().x) / mWindow->drawableSize().y,
													 0.1f, 1000.0f);

			ubo.projMat[1][1] *= -1.0f;

			/*ubo.viewMat.mat = glm::lookAt(glm::vec3(ubo.position.vec),
									  target.vec,
									  glm::vec3(0.0f, 1.0f, 0.0f));

			ubo.projMat.mat = glm::perspective(glm::radians(45.0f),
											   float(mWindow->drawableSize().x) / mWindow->drawableSize().y,
										   0.1f, 1000.0f);

			ubo.projMat[1][1] *= -1.0f;*/

			cameraUniforms[mSwapchain->currentFrame()]->copyTo(&ubo, sizeof(ubo));
		}

		void Vulkan::destroy() {
			mCore->getDevice().waitIdle();

			mCore->getDevice().destroyImageView(mDepthStencilView);
			mCore->getDevice().destroyImage(mDepthStencil.image);
			mCore->getDevice().freeMemory(mDepthStencil.memory);

			mCore->getDevice().destroyImageView(texImageView);
			mCore->getDevice().destroySampler(sampler);

			cameraUniforms.clear();

			for (auto& frameBuffer : mFramebuffers)
				delete frameBuffer;
		}



		void Vulkan::buildCore() {
			mCore->setAppInfo("Demo", Mix::Version::MakeVersion(0, 0, 1));
			mCore->setDebugMode(true);
			mCore->setTargetWindow(mWindow);
			mCore->setQueueFlags();
			mCore->createInstance();
			mCore->pickPhysicalDevice();
			mCore->createLogicalDevice();
			mCore->endInit();
		}

		void Vulkan::buildDebugUtils() {
			mDebug->init(mCore);
			mDebug->setDefaultCallback();
		}

		void Vulkan::buildSwapchain() {
			mSwapchain->init(mCore);
			mSwapchain->setImageCount(2);
			mSwapchain->create(mSwapchain->supportedFormat(),
							   { vk::PresentModeKHR::eFifo },
							   vk::Extent2D(640, 480));
		}

		void Vulkan::buildDepthStencil() {
			mDepthStencil = CreateDepthStencil(*mCore,
											   mSwapchain->extent(),
											   mSettings.sampleCount);

			mDepthStencilView = CreateImageView2D(mCore->getDevice(),
												  mDepthStencil.image,
												  mDepthStencil.format,
												  vk::ImageAspectFlagBits::eDepth |
												  vk::ImageAspectFlagBits::eStencil);
		}

		void Vulkan::buildRenderPass() {
			mRenderPass->init(mCore);
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

		void Vulkan::buildDescriptorSetLayout() {
			auto cameraLayout = std::make_shared<DescriptorSetLayout>();
			cameraLayout->init(mCore);
			cameraLayout->addBindings(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex);
			cameraLayout->addBindings(1, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex);
			cameraLayout->create();

			mDescriptorSetLayout["Camera"] = cameraLayout;
		}

		void Vulkan::buildShaders() {
			mShaderMgr->init(mCore);
			std::ifstream inFile("TestResources/Shaders/vShader.vert.spv", std::ios_base::ate | std::ios_base::binary);
			size_t fileSize = static_cast<uint32_t>(inFile.tellg());
			std::vector<char> shaderCode(fileSize);
			inFile.seekg(std::ios_base::beg);
			inFile.read(shaderCode.data(), fileSize);
			inFile.close();
			mShaderMgr->createShader("vShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eVertex);

			inFile.open("TestResources/Shaders/fShader.frag.spv", std::ios_base::ate | std::ios_base::binary);;
			fileSize = static_cast<uint32_t>(inFile.tellg());
			shaderCode.resize(fileSize);
			inFile.seekg(std::ios_base::beg);
			inFile.read(shaderCode.data(), fileSize);
			inFile.close();
			mShaderMgr->createShader("fShader", shaderCode.data(), shaderCode.size(), vk::ShaderStageFlagBits::eFragment);
		}

		void Vulkan::buildPipeline() {
			mPipelineMgr->init(mCore);
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
			viewPort.width = static_cast<float>(mWindow->drawableSize().x);
			viewPort.height = static_cast<float>(mWindow->drawableSize().y);
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

			pipeline.create();
		}

		void Vulkan::buildCommandMgr() {
			mCommandMgr->init(mCore);
			mCommandMgr->create(vk::QueueFlagBits::eGraphics, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		}

		void Vulkan::buildFrameBuffers() {
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

		void Vulkan::buildUniformBuffers() {
			cameraUniforms.resize(mSwapchain->imageCount());
			dynamicUniformBuffers.resize(mSwapchain->imageCount());

			for (size_t i = 0; i < cameraUniforms.size(); ++i) {
				cameraUniforms[i] = Buffer::CreateBuffer(mCore,
														 mAllocator,
														 vk::BufferUsageFlagBits::eUniformBuffer,
														 vk::MemoryPropertyFlagBits::eHostVisible |
														 vk::MemoryPropertyFlagBits::eHostCoherent,
														 sizeof(Graphics::Uniform::CameraUniform));

				dynamicUniformBuffers[i] = std::make_shared<DynamicUniformBuffer>(mCore,
																				  mAllocator,
																				  sizeof(Uniform::MeshUniform),
																				  100);
			}
		}

		void Vulkan::buildCommandBuffers() {
			mCommandBuffers = mCommandMgr->allocCommandBuffers(mSwapchain->imageCount(),
															   vk::CommandBufferLevel::ePrimary);
		}

		void Vulkan::buildDescriptorSets() {
			//create descriptor pool
			mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, mSwapchain->imageCount() * 5);
			mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBufferDynamic, mSwapchain->imageCount() * 5);
			mDescriptorPool->create(mSwapchain->imageCount() * 5);

			// test Allocate camera descriptor set
			mDescriptorSets = mDescriptorPool->allocDescriptorSet(mDescriptorSetLayout["Camera"]->get(), mSwapchain->imageCount());

			// update descriptor sets
			for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
				std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
				descriptorWrites[0].dstSet = mDescriptorSets[i]; //descriptor which will be write in
				descriptorWrites[0].dstBinding = 0; //destination binding
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer; //the type of the descriptor that will be wirte in
				descriptorWrites[0].descriptorCount = 1; //descriptor count
				descriptorWrites[0].pBufferInfo = &cameraUniforms[i]->descriptor; //descriptorBufferInfo
				descriptorWrites[0].pImageInfo = nullptr;
				descriptorWrites[0].pTexelBufferView = nullptr;


				auto write = dynamicUniformBuffers[i]->getWriteDescriptorSet(mDescriptorSets[i], 1);
				descriptorWrites[1] = write.get();

				mCore->getDevice().updateDescriptorSets(descriptorWrites, nullptr);
			}
		}

		void Vulkan::loadResource() {
			mImageMgr->init(mCore, mAllocator);

			/*const gli::texture2d texture(gli::load("TestResources/Texture/1.DDS"));
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

			texImageView = mCore->getDevice().createImageView(viewInfo);

			vk::SamplerCreateInfo samplerInfo;
			samplerInfo.magFilter = vk::Filter::eLinear;
			samplerInfo.minFilter = vk::Filter::eLinear;
			samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
			samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
			samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
			samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;

			sampler = mCore->getDevice().createSampler(samplerInfo);*/
		}
	}
}

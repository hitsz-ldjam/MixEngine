#include"MxVulkan.h"
#include"MxCamera.h"
#include"MxWindow.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>
#include<array>
#include<vector>
#include<chrono>

#define MAX_FRAMES_IN_FLIGHT 2

struct UniformBufferObj
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class TestDemo
{
private:
	Mix::MxWindow* mWindow;
	Mix::MxVkManager* mManager;
	Mix::MxVkDebug* mDebug;
	Mix::MxVkShaderHelper* mShaderHelper;
	Mix::MxVkSwapchain* mSwapchain;
	Mix::MxVkRenderPass* mRenderPass;
	Mix::MxVkDescriptorSetLayout* mDescriptorSetLayout;
	Mix::MxVkDescriptorPool* mDescriptorPool;
	Mix::MxVkPipeline* mPipeline;
	Mix::MxVkCommandPool* mCommandPool;
	std::vector<Mix::MxVkFramebuffer*> mFramebuffers;
	Mix::MxVkSyncObjectPool* mSyncObjectPool;

	VkSampleCountFlagBits mSampleCount;
	VkViewport mViewport;
	VkRect2D mScissor;
	Mix::MxVkImage* mDepthImage;

	Mix::MxVkBuffer* mVertexBuffer;
	std::vector<Mix::MxVkBuffer*> mUniformBuffers;
	std::vector<VkDescriptorSet> mDescriptorSets;
	std::vector<VkCommandBuffer> mCommandBuffers;
	std::vector<VkSemaphore> mImageAvailableSemaphores;
	std::vector<VkSemaphore> mRenderFinishedSemaphores;
	std::vector<VkFence> mInFlightFences;

	size_t mCurrFrame;

	Mix::MxVkModelImporter* mModelImporter;
	Mix::MxVkModel* mModel;

	Mix::MxCamera mCamera;

	void draw();
	void updateUniformBuffer(const uint32_t imageIndex);
public:
	TestDemo();
	~TestDemo() = default;
	bool init();
	void run();
	void destroy();
};

//debug test----------------------------------
#include"MxUtils.h"
void drawSlipLine()
{
	std::cout << "------------------------" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec3)
{
	return os << "[ " << vec3.x << ", " << vec3.y << ", " << vec3.z << " ]";
}

std::ostream& operator<<(std::ostream& os, const Mix::MxAxis& axis)
{
	return os << "{ " << axis.x << ", " << axis.y << ", " << axis.z << " }";
}

std::ostream& operator<<(std::ostream& os, const glm::quat& quat)
{
	return os << "[ " << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << " ]";
}

std::ostream& operator<<(std::ostream& os, const Mix::MxTransform& trans)
{
	std::cout << trans.getPosition() << std::endl;
	std::cout << trans.getAxis() << std::endl;
	return std::cout << trans.getEulerAngles() << std::endl;
}
//debug test----------------------------------
int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "Error : Failed to initialize SDL2!" << std::endl;
		return -1;
	}

	TestDemo demo;
	demo.init();
	demo.run();
	demo.destroy();
	return 0;
}

void TestDemo::run()
{
	bool quit = false;
	SDL_Event event;
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
				break;
			}

			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		draw();
	}
}

void TestDemo::draw()
{
	vkWaitForFences(mManager->getDevice(), 1, &mInFlightFences[mCurrFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mManager->getDevice(), mSwapchain->getSwapchain(), std::numeric_limits<uint64_t>::max(),
											mImageAvailableSemaphores[mCurrFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		std::cerr << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
		return;
	} else
		MX_VK_CHECK_RESULT(result);

	updateUniformBuffer(imageIndex);

	//submit command buffer
	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrFrame] };
	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.pSignalSemaphores = signalSemaphores;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];
	submitInfo.commandBufferCount = 1;

	MX_VK_CHECK_RESULT(vkResetFences(mManager->getDevice(), 1, &mInFlightFences[mCurrFrame]));

	MX_VK_CHECK_RESULT(vkQueueSubmit(mManager->getQueue().graphics, 1, &submitInfo, mInFlightFences[mCurrFrame]));

	//swapchain present
	VkSwapchainKHR swapChains[] = { mSwapchain->getSwapchain() };

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.swapchainCount = 1;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(mManager->getQueue().present, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		std::cerr << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
		return;
	} else
		MX_VK_CHECK_RESULT(result);

	mCurrFrame = (mCurrFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void TestDemo::updateUniformBuffer(const uint32_t imageIndex)
{
	// test quaternion
	static auto start = std::chrono::system_clock::now();
	auto duration = std::chrono::system_clock::now() - start;

	auto angle = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() * 0.08f;

	glm::vec3 eulerAngle(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f));
	glm::quat quaternion = glm::tquat(eulerAngle);

	UniformBufferObj ubo = {};
	ubo.model = glm::mat4(1.0f)*glm::toMat4(quaternion);

	mCamera.setPosition(0.0f, angle*0.008f + -5.0f, 3.0f);
	mCamera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	ubo.view = std::move(mCamera.getViewMatrix());

	ubo.proj = glm::perspective(glm::radians(45.0f),
								mSwapchain->getCurrExtent().width / float(mSwapchain->getCurrExtent().height),
								0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	mUniformBuffers[imageIndex]->map();
	mUniformBuffers[imageIndex]->copyTo(&ubo, sizeof(ubo));
	mUniformBuffers[imageIndex]->unmap();
}

TestDemo::TestDemo()
{
	mWindow = new Mix::MxWindow;
	mManager = new Mix::MxVkManager;
	mDebug = new Mix::MxVkDebug;
	mShaderHelper = new Mix::MxVkShaderHelper;
	mSwapchain = new Mix::MxVkSwapchain;
	mRenderPass = new Mix::MxVkRenderPass;
	mDescriptorPool = new Mix::MxVkDescriptorPool;
	mDescriptorSetLayout = new Mix::MxVkDescriptorSetLayout;
	mPipeline = new Mix::MxVkPipeline;
	mCommandPool = new Mix::MxVkCommandPool;
	mSyncObjectPool = new Mix::MxVkSyncObjectPool;

	mSampleCount = VK_SAMPLE_COUNT_1_BIT;
	mCurrFrame = 0;

	mModelImporter = new Mix::MxVkModelImporter;
	if (!mModelImporter->initialize())
		throw std::runtime_error("Error : Failed to create model importer");
	mModel = mModelImporter->loadModel("Model\\robot.fbx");

}

bool TestDemo::init()
{
	mWindow->create("Test Demo", 600, 480);

	auto info = mManager->getEmptyInitInfo();

	info->debugMode = true;
	info->present = true;

	info->instance.validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	info->instance.appInfo.appName = mWindow->getTitle();
	info->instance.appInfo.appVersion = VK_MAKE_VERSION(0, 0, 1);
	info->instance.appInfo.engineName = "Mix";
	info->instance.appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);

	info->device.physical.type = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	info->device.physical.enabledFeatures = { false };
	info->device.physical.extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	info->device.physical.queueFlags = VK_QUEUE_GRAPHICS_BIT;
	info->device.physical.queuePrioriy.graphics = 1.0f;

	info->window = mWindow->getWindow();

	try
	{
		mManager->initialize(*info);

		mDebug->setup(mManager);
		mShaderHelper->setup(mManager);

		//setup debug
		mDebug->setDefaultCallback(Mix::MxVkDebug::SEVERITY_ALL, Mix::MxVkDebug::TYPE_ALL);

		//setup swapchain
		auto rect = mWindow->getWindowRect();
		mSwapchain->setup(mManager);
		mSwapchain->createSwapchain({ {VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR } },
									VK_PRESENT_MODE_MAILBOX_KHR,
									{ static_cast<uint32_t>(rect.width),static_cast<uint32_t>(rect.height) });

		//setup renderpass
		mRenderPass->setup(mManager);
		auto presentAttach = mRenderPass->addColorAttach(mSwapchain->getCurrFormat().format, mSampleCount,
														 VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
														 VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		auto presentAttachRef = mRenderPass->addColorAttachRef(presentAttach);

		auto depthAttach = mRenderPass->addDepthStencilAttach(VK_FORMAT_D24_UNORM_S8_UINT, mSampleCount);
		auto depthAttachRef = mRenderPass->addDepthStencilAttachRef(depthAttach);

		auto subpass = mRenderPass->addSubpass();
		mRenderPass->addSubpassColorRef(subpass, presentAttachRef);
		mRenderPass->addSubpassDepthStencilRef(subpass, depthAttachRef);

		mRenderPass->addDependency(VK_SUBPASS_EXTERNAL, subpass,
								   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								   0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
		mRenderPass->createRenderPass();

		//setup descriptor
		mDescriptorSetLayout->setup(mManager);
		mDescriptorSetLayout->addBindings(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT);
		mDescriptorSetLayout->createDescriptorSetLayout();

		//load shader
		mShaderHelper->setup(mManager);
		auto vertexShader = mShaderHelper->createModule("Shader/vShader.spv", VK_SHADER_STAGE_VERTEX_BIT);
		auto fragShader = mShaderHelper->createModule("Shader/fShader.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

		//setup graphics pipeline
		mPipeline->setup(mManager);
		mPipeline->setTargetRenderPass(mRenderPass->getRenderPass(), 0);

		mPipeline->addShader(vertexShader->stage, vertexShader->module);
		mPipeline->addShader(fragShader->stage, fragShader->module);

		//setup vertex input state
		//todo create a new class to deal with this
		std::vector<VkVertexInputBindingDescription> inputBinding;
		std::vector<VkVertexInputAttributeDescription> inputAttri;
		inputBinding.push_back(Mix::Vertex::getBindingDescription());
		auto attri = Mix::Vertex::getAttributeDescription();
		inputAttri.insert(inputAttri.end(), attri.begin(), attri.end());
		mPipeline->setVertexInput(inputBinding, inputAttri);

		//setup input assembly
		mPipeline->setInputAssembly();

		//setup viewport
		mViewport.x = 0;
		mViewport.y = 0;
		mViewport.width = static_cast<float>(mWindow->getWindowRect().width);
		mViewport.height = static_cast<float>(mWindow->getWindowRect().height);
		mViewport.minDepth = 0.0f;
		mViewport.maxDepth = 1.0f;
		mPipeline->addViewport(mViewport);

		mScissor.extent = mSwapchain->getCurrExtent();
		mScissor.offset = { 0,0 };
		mPipeline->addScissor(mScissor);

		//setup rasterization
		mPipeline->setRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
		mPipeline->setDepthBias(false);

		//setup multisample
		mPipeline->setMultiSample(mSampleCount);

		//setup depth/stencil test
		mPipeline->setDepthTest(true);
		mPipeline->setDepthBoundsTest(false);
		mPipeline->setStencilTest(false);

		//setup blend
		mPipeline->addDefaultBlendAttachments();

		//setup layout
		mPipeline->addDescriptorSetLayout(mDescriptorSetLayout->getDescriptorSetLayout());
		mPipeline->addPushConstantRanges();

		//create graphics pipeline
		mPipeline->createPipeline();

		//create depth stencil buffer
		mDepthImage = Mix::MxVkImage::createDepthStencil(mManager, VK_FORMAT_D24_UNORM_S8_UINT,
														 mSwapchain->getCurrExtent(),
														 mSampleCount);

	  //create command pool
		mCommandPool->setup(mManager);
		mCommandPool->createCommandPool(VK_QUEUE_GRAPHICS_BIT);

		//setup framebuffer
		std::vector<VkImageView> attachments;
		mFramebuffers.resize(mSwapchain->getImageCount());
		for (uint32_t i = 0; i < mFramebuffers.size(); ++i)
		{
			attachments = { mSwapchain->getImageViews()[i],mDepthImage->view };

			mFramebuffers[i] = new Mix::MxVkFramebuffer;
			mFramebuffers[i]->setup(mManager);
			mFramebuffers[i]->setExtent({ static_cast<uint32_t>(rect.width),static_cast<uint32_t>(rect.height) });
			mFramebuffers[i]->setLayers(1);
			mFramebuffers[i]->setTargetRenderPass(mRenderPass->getRenderPass());
			mFramebuffers[i]->addAttachments(attachments);
			mFramebuffers[i]->createFramebuffer();
		}

		mModel->createVertexBuffer(mManager, mCommandPool);

		//create uniform buffer
		{
			mUniformBuffers.resize(mSwapchain->getImageCount());
			for (size_t i = 0; i < mSwapchain->getImageCount(); ++i)
			{
				mUniformBuffers[i] = Mix::MxVkBuffer::createBuffer(mManager, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
																   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
																   sizeof(UniformBufferObj));
			}
		}

		//create descriptor pool
		mDescriptorPool->setup(mManager);
		mDescriptorPool->addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, mSwapchain->getImageCount());
		mDescriptorPool->createDescriptorPool(mSwapchain->getImageCount());

		//allocate descriptor sets
		mDescriptorSets = mDescriptorPool->allocDescriptorSet(*mDescriptorSetLayout, mSwapchain->getImageCount());

		//update descriptor sets
		for (size_t i = 0; i < mSwapchain->getImageCount(); ++i)
		{
			std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = mDescriptorSets[i]; //descriptor which will be write in
			descriptorWrites[0].dstBinding = 0; //destination binding
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; //the type of the descriptor that will be wirte in
			descriptorWrites[0].descriptorCount = 1; //descriptor count
			descriptorWrites[0].pBufferInfo = &mUniformBuffers[i]->descriptor; //descriptorBufferInfo
			descriptorWrites[0].pImageInfo = nullptr; 
			descriptorWrites[0].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(mManager->getDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}

		//create commandbuffer
		{
			mCommandBuffers = mCommandPool->allocCommandBuffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, mSwapchain->getImageCount());

			for (size_t i = 0; i < mCommandBuffers.size(); ++i)
			{
				//begin command buffer
				Mix::beginCommandBuffer(mCommandBuffers[i]);

				std::vector<VkClearValue> clearValues(2);
				clearValues[0].color = { 0.0f,0.0f,0.0f,1.0f };
				clearValues[1].depthStencil = { 1.0f,0 };

				//begin render pass
				mRenderPass->beginRenderPass(mCommandBuffers[i], mFramebuffers[i]->getFramebuffer(), clearValues,
											 mSwapchain->getCurrExtent());

				//bind pipeline
				vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->getPipeline());

				//bind descriptor sets
				vkCmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline->getPipelineLayout(),
										0, // the index of the first descriptor set
										1, // descriptor count
										&mDescriptorSets[i], // descriptor array
										0, nullptr);

				//draw
				mModel->draw(mCommandBuffers[i]);

				//end render pass
				mRenderPass->endRenderPass(mCommandBuffers[i]);

				//end command buffer
				Mix::endCommandBuffer(mCommandBuffers[i]);
			}
		}

		//create fence and semaphore
		mSyncObjectPool->setup(mManager);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			mImageAvailableSemaphores.push_back(mSyncObjectPool->createSemaphore());
			mRenderFinishedSemaphores.push_back(mSyncObjectPool->createSemaphore());
			mInFlightFences.push_back(mSyncObjectPool->createFence());
		}
	}

	catch (const std::exception& e)
	{
		std::cout << e.what() << std::ends << std::endl;
	}

	return true;
}

void TestDemo::destroy()
{
	vkDeviceWaitIdle(mManager->getDevice());

	MX_FREE_OBJECT(mSyncObjectPool);
	MX_FREE_OBJECT(mCommandPool);

	vkDestroyImageView(mManager->getDevice(), mDepthImage->view, nullptr);
	vkDestroyImage(mManager->getDevice(), mDepthImage->image, nullptr);
	vkFreeMemory(mManager->getDevice(), mDepthImage->memory, nullptr);

	for (auto& framebuffer : mFramebuffers)
		MX_FREE_OBJECT(framebuffer);

	MX_FREE_OBJECT(mShaderHelper);
	MX_FREE_OBJECT(mSwapchain);
	MX_FREE_OBJECT(mDescriptorSetLayout);
	MX_FREE_OBJECT(mDescriptorPool);
	MX_FREE_OBJECT(mPipeline);
	MX_FREE_OBJECT(mRenderPass);

	MX_FREE_OBJECT(mModel);
	for (auto& buffer : mUniformBuffers)
		MX_FREE_OBJECT(buffer);

	MX_FREE_OBJECT(mDebug);
	MX_FREE_OBJECT(mManager);
	MX_FREE_OBJECT(mWindow);
}
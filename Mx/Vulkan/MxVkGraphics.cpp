#include "MxVkGraphics.h"

#include "Instance/MxVkInstance.h"
#include "Device/MxVkPhysicalDevice.h"
#include "Device/MxVkDevice.h"
#include "Debug/MxVkDebug.h"
#include "Swapchain/MxVkSwapchain.h"
#include "CommandBuffer/MxVkCommandPool.h"
#include "Descriptor/MxVkDescriptor.h"

namespace Mix {
	namespace Graphics {
		void Vulkan::init() {
			vk::ApplicationInfo appInfo;
			appInfo.apiVersion = VK_VERSION_1_1;
			appInfo.pEngineName = EngineInfo::EngineName.c_str();
			appInfo.engineVersion = EngineInfo::EngineVersion;
			appInfo.pApplicationName = "";
			appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);

			mInstance = std::make_shared<Instance>(appInfo);

			mPhysicalDeviceInfos = std::make_shared<std::vector<PhysicalDeviceInfo>>();

			auto physicalDevices = mInstance->get().enumeratePhysicalDevices();
			for (auto& physicalDevice : physicalDevices) {
				mPhysicalDeviceInfos->emplace_back();
				auto& info = mPhysicalDeviceInfos->back();
				info.physicalDevice = physicalDevice;

				info.properties = physicalDevice.getProperties();
				info.features = physicalDevice.getFeatures();
				info.extensions = physicalDevice.enumerateDeviceExtensionProperties();
				info.queueFamilies = physicalDevice.getQueueFamilyProperties();
				info.memoryProperties = physicalDevice.getMemoryProperties();
			}

			mInstance.reset();
		}

		std::vector<vk::ExtensionProperties> Vulkan::GetAllSupportedInstanceExts() {
			return Instance::GetSupportedExts();
		}

		std::vector<vk::LayerProperties> Vulkan::GetAllSupportedLayers() {
			return Instance::GetSupportedLayers();
		}

		void Vulkan::build(const VulkanSettings& _settings) {
			mSettings = std::make_shared<VulkanSettings>(_settings);
			build();
		}

		void Vulkan::build(const VulkanSettings* _settings) {
			mSettings = std::make_shared<VulkanSettings>(*_settings);
			build();
		}

		void Vulkan::build() {
			createInstance();
			pickPhysicalDevice();
			createDevice();
			createDebugUtils();
			createDescriptorPool();
			createSwapchain();
			createCommandPool();
			createAllocator();

			mRendererHolder = std::make_shared<RendererHolder>();

			mGraphicsCommandBuffers.reserve(mSwapchain->imageCount());
			for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
				mGraphicsCommandBuffers.emplace_back(mGraphicsCommandPool);
			}
		}

		void Vulkan::update() {
			mRendererHolder->getRenderer(mActiveRenderer)->update(mSwapchain->currFrame());
		}

		void Vulkan::render() {
			auto currFrame = mSwapchain->currFrame();
			mGraphicsCommandBuffers[currFrame].wait();

			mSwapchain->acquireNextImage();

			mGraphicsCommandBuffers[currFrame].begin();
			mRendererHolder->getRenderer(mActiveRenderer)->render(mGraphicsCommandBuffers[currFrame], currFrame);
			mGraphicsCommandBuffers[currFrame].end();

			mGraphicsCommandBuffers[currFrame].submit({ mSwapchain->presentFinishedSph() }, // wait for image
													  { vk::PipelineStageFlagBits::eColorAttachmentOutput },
													  { mSwapchain->renderFinishedSph() }); // notify swapchain

			mSwapchain->present();
		}

		uint32_t Vulkan::addRenderer(RendererBase* _renderer) {
			if (_renderer) {
				_renderer->setVulkanPointer(this);

				return mRendererHolder->addRenderer(_renderer);
			}
			return 0;
		}

		RendererBase* Vulkan::getRenderer(const uint32_t _index) const {
			return mRendererHolder->getRenderer(_index);
		}

		void Vulkan::removeRenderer(const uint32_t _index) const {
			mRendererHolder->removeRenderer(_index);
		}

		Vulkan::~Vulkan() {
			mDevice->get().waitIdle();
			mRendererHolder->clear();

			mGraphicsCommandBuffers.clear();

			mGraphicsCommandPool.reset();
			mTransferCommandPool.reset();
			mSwapchain.reset();
			mAllocator.reset();
			mDebugUtils.reset();
			mPhysicalDevice.reset();
			mDevice.reset();
			if (mSurface)
				mInstance->get().destroySurfaceKHR(mSurface);
			mInstance.reset();
		}

		void Vulkan::createInstance() {
			vk::ApplicationInfo appInfo;
			appInfo.apiVersion = VK_VERSION_1_1;
			appInfo.pEngineName = EngineInfo::EngineName.c_str();
			appInfo.engineVersion = EngineInfo::EngineVersion;
			appInfo.pApplicationName = mSettings->appInfo.appName.c_str();
			appInfo.applicationVersion = mSettings->appInfo.appVersion;

			mInstance = std::make_shared<Instance>(appInfo,
												   mSettings->instanceExts,
												   mSettings->debugMode,
												   mSettings->validationLayers);
		}

		void Vulkan::pickPhysicalDevice() {
			const auto index = mSettings->physicalDeviceIndex < mPhysicalDeviceInfos->size() ?
				mSettings->physicalDeviceIndex : 0;
			auto devices = mInstance->get().enumeratePhysicalDevices();
			mPhysicalDevice = std::make_shared<PhysicalDevice>(mInstance,
															   devices[index]);
		}

		void Vulkan::createDevice() {
			VkSurfaceKHR surface;
			SDL_Vulkan_CreateSurface(mWindow->window(), static_cast<VkInstance>(mInstance->get()), &surface);
			mSurface = static_cast<vk::SurfaceKHR>(surface);
			mDevice = std::make_shared<Device>(mPhysicalDevice,
											   mSurface,
											   &mSettings->enabledFeatures,
											   mSettings->deviceExts,
											   mSettings->validationLayers,
											   vk::QueueFlagBits::eTransfer |
											   vk::QueueFlagBits::eGraphics);
		}

		void Vulkan::createDebugUtils() {
			mDebugUtils = std::make_shared<DebugUtils>(mDevice);
			/*mDebugUtils->addDefaultCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
											vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);*/
			mDebugUtils->addDefaultCallback();
		}

		void Vulkan::createDescriptorPool() {
			mDescriptorPool = std::make_shared<DescriptorPool>(mDevice);
			mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, 64);
			mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBufferDynamic, 32);
			mDescriptorPool->addPoolSize(vk::DescriptorType::eCombinedImageSampler, 128);
			mDescriptorPool->create(128);
		}

		void Vulkan::createSwapchain() {
			mSwapchain = std::make_shared<Swapchain>(mDevice);
			mSwapchain->setImageCount(2);
			mSwapchain->create(mSwapchain->supportedFormat(),
							   { vk::PresentModeKHR::eFifo },
							   vk::Extent2D(640, 480));
		}

		void Vulkan::createCommandPool() {
			mTransferCommandPool = std::make_shared<CommandPool>(mDevice, vk::QueueFlagBits::eTransfer);
			mGraphicsCommandPool = std::make_shared<CommandPool>(mDevice, vk::QueueFlagBits::eGraphics,
																 vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		}

		void Vulkan::createAllocator() {
			mAllocator = std::make_shared<DeviceAllocator>(mDevice);
		}
	}
}

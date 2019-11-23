#include "MxVulkan.h"

#include "Instance/MxVkInstance.h"
#include "Device/MxVkPhysicalDevice.h"
#include "Device/MxVkDevice.h"
#include "Debug/MxVkDebug.h"
#include "Swapchain/MxVkSwapchain.h"
#include "CommandBuffer/MxVkCommandPool.h"
#include "Descriptor/MxVkDescriptorSet.h"
#include "Shader/MxVkShaderBase.h"
#include "Memory/MxVkAllocator.h"
#include "Pipeline/MxVkVertexInput.h"
#include "../Definitions/MxSystemInfo.h"
#include "MxVkUtils.h"
#include "Image/MxVkImage.h"
#include "FrameBuffer/MxVkFramebuffer.h"

namespace Mix {
    namespace Vulkan {
        std::string VulkanAPI::getApiName() const {
            return "Vulkan";
        }

        std::shared_ptr<CommandBuffer> VulkanAPI::getMainCmdBuffer() const {
        }

        void VulkanAPI::init() {
            vk::ApplicationInfo appInfo;
            appInfo.apiVersion = VK_API_VERSION_1_1;
            appInfo.pEngineName = SystemInfo::EngineName.c_str();
            appInfo.engineVersion = VulkanUtils::CvtToVkVersion(SystemInfo::EngineVersion);
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

        std::vector<vk::ExtensionProperties> VulkanAPI::GetAllSupportedInstanceExts() {
            return Instance::GetSupportedExts();
        }

        std::vector<vk::LayerProperties> VulkanAPI::GetAllSupportedLayers() {
            return Instance::GetSupportedLayers();
        }

        void VulkanAPI::build(const VulkanSettings& _settings) {
            mSettings = std::make_shared<VulkanSettings>(_settings);
            build();
        }

        void VulkanAPI::build() {
            // Initialize Vulkan API
            createInstance();
            pickPhysicalDevice();
            createDevice();
            createDebugUtils();
            createDescriptorPool();
            createSwapchain();
            createCommandPool();
            createAllocator();
            createRenderPass();
            createFrameBuffer();

            mGraphicsCommandBuffers.reserve(mSwapchain->imageCount());
            for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
                mGraphicsCommandBuffers.emplace_back(std::make_shared<CommandBufferHandle>(mGraphicsCommandPool));
            }

            mVertexInputManager = std::make_shared<VertexInputManager>();
        }

        void VulkanAPI::beginRender() {
            mCurrFrame = mSwapchain->getCurrFrame();
            mCurrCmd = mGraphicsCommandBuffers[mCurrFrame].get();
            mCurrCmd->wait();
            mSwapchain->acquireNextImage();
            mCurrCmd->begin();

            std::vector<vk::ClearValue> clearValues(2);
            clearValues[0].color = std::array<float, 4>{0.2f, 0.2f, 0.2f, 1.0f};
            clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

            mRenderPass->beginRenderPass(mCurrCmd->get(),
                                         mFrameBuffers[mCurrFrame].get(),
                                         clearValues,
                                         mSwapchain->extent());
        }

        void VulkanAPI::endRender() {
            mRenderPass->endRenderPass(mCurrCmd->get());

            mCurrCmd->end();
            mCurrCmd->submit({ mSwapchain->presentFinishedSph() }, // wait for image
                             { vk::PipelineStageFlagBits::eColorAttachmentOutput },
                             { mSwapchain->renderFinishedSph() }); // notify swapchain
            mSwapchain->present();
        }

        void VulkanAPI::waitDeviceIdle() {
            mDevice->getVkHandle().waitIdle();
        }

        VulkanAPI::~VulkanAPI() {
            try {
                mDevice->getVkHandle().waitIdle();
            }
            catch (vk::Error& e) {
                std::cerr << e.what() << std::endl;
            }

            if (mDevice && mDepthStencilView)
                mDevice->getVkHandle().destroy(mDepthStencilView);

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

        void VulkanAPI::createInstance() {
            vk::ApplicationInfo appInfo;
            appInfo.apiVersion = VK_API_VERSION_1_1;
            appInfo.pEngineName = SystemInfo::EngineName.c_str();
            appInfo.engineVersion = VulkanUtils::CvtToVkVersion(SystemInfo::EngineVersion);
            appInfo.pApplicationName = mSettings->appInfo.appName.c_str();
            appInfo.applicationVersion = VulkanUtils::CvtToVkVersion(mSettings->appInfo.appVersion);

            mInstance = std::make_shared<Instance>(appInfo,
                                                   mSettings->instanceExts,
                                                   mSettings->debugMode,
                                                   mSettings->validationLayers);
        }

        void VulkanAPI::pickPhysicalDevice() {
            const auto index = mSettings->physicalDeviceIndex < mPhysicalDeviceInfos->size() ? mSettings->physicalDeviceIndex : 0;
            auto devices = mInstance->get().enumeratePhysicalDevices();
            mPhysicalDevice = std::make_shared<PhysicalDevice>(mInstance,
                                                               devices[index]);
        }

        void VulkanAPI::createDevice() {
            VkSurfaceKHR surface;
            SDL_Vulkan_CreateSurface(mWindow->rawPtr(), static_cast<VkInstance>(mInstance->get()), &surface);
            mSurface = static_cast<vk::SurfaceKHR>(surface);
            mDevice = std::make_shared<Device>(mPhysicalDevice,
                                               mSurface,
                                               &mSettings->enabledFeatures,
                                               mSettings->deviceExts,
                                               mSettings->validationLayers,
                                               vk::QueueFlagBits::eTransfer |
                                               vk::QueueFlagBits::eGraphics);
        }

        void VulkanAPI::createDebugUtils() {
            mDebugUtils = std::make_shared<DebugUtils>(mDevice);
            /*mDebugUtils->addDefaultCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);*/
            mDebugUtils->addDefaultCallback();
        }

        void VulkanAPI::createDescriptorPool() {
            mDescriptorPool = std::make_shared<DescriptorPool>(mDevice);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBuffer, 64);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eUniformBufferDynamic, 32);
            mDescriptorPool->addPoolSize(vk::DescriptorType::eCombinedImageSampler, 128);
            mDescriptorPool->create(128);
        }

        void VulkanAPI::createSwapchain() {
            mSwapchain = std::make_shared<Swapchain>(mDevice);
            mSwapchain->setImageCount(2);
            mSwapchain->create(mSwapchain->supportedFormat(),
                               { vk::PresentModeKHR::eFifo },
                               vk::Extent2D(640, 480));
        }

        void VulkanAPI::createCommandPool() {
            mTransferCommandPool = std::make_shared<CommandPool>(mDevice, vk::QueueFlagBits::eTransfer);
            mGraphicsCommandPool = std::make_shared<CommandPool>(mDevice, vk::QueueFlagBits::eGraphics,
                                                                 vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        }

        void VulkanAPI::createAllocator() {
            mAllocator = std::make_shared<DeviceAllocator>(mDevice);
        }

        void VulkanAPI::createRenderPass() {
            mDepthStencil = Image::CreateDepthStencil(getAllocator(),
                                                      mSwapchain->extent(),
                                                      vk::SampleCountFlagBits::e1);
            mDepthStencilView = Image::CreateVkImageView2D(mDevice->getVkHandle(),
                                                           mDepthStencil->get(),
                                                           mDepthStencil->format(),
                                                           vk::ImageAspectFlagBits::eDepth |
                                                           vk::ImageAspectFlagBits::eStencil);

            // RenderPass
            mRenderPass = std::make_shared<RenderPass>(getLogicalDevice());

            mRenderPass->addAttachment({
                {0, Attachment::Type::PRESENT, mSwapchain->surfaceFormat().format},
                {1, Attachment::Type::DEPTH_STENCIL, mDepthStencil->format()}
                                       });

            Subpass subpass(0);
            subpass.addRef({
                {AttachmentRef::Type::COLOR, 0},
                {AttachmentRef::Type::DEPTH_STENCIL, 1}
                           });

            mRenderPass->addSubpass(subpass);

            mRenderPass->addDependency({
                VK_SUBPASS_EXTERNAL,
                0,
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::PipelineStageFlagBits::eColorAttachmentOutput,
                vk::AccessFlags(),
                vk::AccessFlagBits::eColorAttachmentRead |
                vk::AccessFlagBits::eColorAttachmentWrite
                                       });
            mRenderPass->create();
        }

        void VulkanAPI::createFrameBuffer() {
            for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
                mFrameBuffers.emplace_back(mRenderPass, getSwapchain()->extent());
                mFrameBuffers.back().addAttachments({ mSwapchain->getImageViews()[i], mDepthStencilView });
                mFrameBuffers.back().create();
            }
        }

        void VulkanAPI::destroy() {
        }
    }
}

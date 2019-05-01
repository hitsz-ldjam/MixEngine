#include "MxVkCore.h"

namespace Mix {
    namespace Graphics {
        Core::Core()
            :mWindow(nullptr) {
            mInitInfo = new InitInfo();
        }

        void Core::createInstance() {
            // setup application info
            vk::ApplicationInfo appInfo(mAppName.c_str(),
                                        mAppVersion,
                                        EngineInfo::engineName.c_str(),
                                        EngineInfo::engineVersion,
                                        VK_API_VERSION_1_1);

            // setup instance create info
            vk::InstanceCreateInfo createInfo;
            createInfo.pApplicationInfo = &appInfo;

            // if in debug mode
            if (mInitInfo->debugMode) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(mInitInfo->validationLayers.size());
                createInfo.ppEnabledLayerNames = mInitInfo->validationLayers.data();
            } else {
                createInfo.enabledLayerCount = 0;
                createInfo.ppEnabledLayerNames = nullptr;
            }

            createInfo.enabledExtensionCount = static_cast<uint32_t>(mInitInfo->instanceExtensions.size());
            createInfo.ppEnabledExtensionNames = mInitInfo->instanceExtensions.data();

            mInstance = vk::createInstance(createInfo);

            VkSurfaceKHR surface;
            SDL_Vulkan_CreateSurface(mWindow->getWindowPtr(), static_cast<VkInstance>(mInstance), &surface);
            mSurface = static_cast<vk::SurfaceKHR>(surface);

            mSupportedExtensions = getSupportedExtensions();
            mSupportedLayers = getSupportedLayers();

            auto physicalDevices = mInstance.enumeratePhysicalDevices();
            for (auto& physicalDevice : physicalDevices) {
                PhysicalDeviceInfo info;
                info.physicalDevice = physicalDevice;

                info.properties = physicalDevice.getProperties();
                info.features = physicalDevice.getFeatures();
                info.extensions = physicalDevice.enumerateDeviceExtensionProperties();
                info.queueFamilies = physicalDevice.getQueueFamilyProperties();
                info.familyIndexSet = getQueueFamilyIndexSet(info);
                info.memoryProperties = physicalDevice.getMemoryProperties();

                mAllPhysicalDevices.push_back(std::move(info));
            }
        }

        void Core::pickPhysicalDevice(const std::function<uint32_t(const PhysicalDeviceInfo&)>& marker) {
            const PhysicalDeviceInfo* top = nullptr;
            uint32_t topScore = 0;

            for (auto& deviceInfo : mAllPhysicalDevices) {
                // pick the physical device with the highest score
                uint32_t score = marker ? marker(deviceInfo) : evaluatePhysicalDevice(deviceInfo);
                if (topScore <= score) {
                    topScore = score;
                    top = &deviceInfo;
                }
            }

            if (!top)
                throw NoSuitableVulkanDevice();

            mPhysicalDeviceInfo = *top;

        }

        void Core::createLogicalDevice() {
            auto& physicalDevice = mPhysicalDeviceInfo.physicalDevice;
            auto physicalDeviceInfo = std::find_if(mAllPhysicalDevices.begin(),
                                                   mAllPhysicalDevices.end(),
                                                   [&physicalDevice](const PhysicalDeviceInfo& info)->bool {
                                                       return info.physicalDevice == physicalDevice;
                                                   });

            std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
            std::map<uint32_t, float> uniqueQueueFamilies;

            if (physicalDeviceInfo->familyIndexSet.graphics)
                uniqueQueueFamilies[physicalDeviceInfo->familyIndexSet.graphics.value()] = 1.0f;
            if (physicalDeviceInfo->familyIndexSet.compute)
                uniqueQueueFamilies[physicalDeviceInfo->familyIndexSet.compute.value()] = 1.0f;
            if (physicalDeviceInfo->familyIndexSet.present)
                uniqueQueueFamilies[physicalDeviceInfo->familyIndexSet.present.value()] = 1.0f;
            if (physicalDeviceInfo->familyIndexSet.transfer)
                uniqueQueueFamilies[physicalDeviceInfo->familyIndexSet.transfer.value()] = 1.0f;

            for (const auto& pair : uniqueQueueFamilies) {
                vk::DeviceQueueCreateInfo createInfo;
                createInfo.queueFamilyIndex = pair.first;
                createInfo.pQueuePriorities = &pair.second;
                createInfo.queueCount = 1;
                queueCreateInfos.push_back(std::move(createInfo));
            }

            vk::DeviceCreateInfo createInfo;
            createInfo.pQueueCreateInfos = queueCreateInfos.data();
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            createInfo.pEnabledFeatures = &mInitInfo->physicalDeviceFeatures;

            mInitInfo->deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            createInfo.enabledExtensionCount = static_cast<uint32_t>(mInitInfo->deviceExtensions.size());
            createInfo.ppEnabledExtensionNames = mInitInfo->deviceExtensions.data();

            if (mInitInfo->debugMode) {
                createInfo.enabledLayerCount = static_cast<uint32_t>(mInitInfo->validationLayers.size());
                createInfo.ppEnabledLayerNames = mInitInfo->validationLayers.data();
            } else {
                createInfo.ppEnabledLayerNames = nullptr;
                createInfo.enabledLayerCount = 0;
            }

            mLogicalDevice = physicalDevice.createDevice(createInfo);

            mQueueSet.present = mLogicalDevice.getQueue(physicalDeviceInfo->familyIndexSet.present.value(), 0);
            if (mInitInfo->queueFlags & vk::QueueFlagBits::eGraphics)
                mQueueSet.graphics = mLogicalDevice.getQueue(physicalDeviceInfo->familyIndexSet.graphics.value(), 0);
            if (mInitInfo->queueFlags & vk::QueueFlagBits::eCompute)
                mQueueSet.compute = mLogicalDevice.getQueue(physicalDeviceInfo->familyIndexSet.compute.value(), 0);
            if (mInitInfo->queueFlags & vk::QueueFlagBits::eTransfer)
                mQueueSet.transfer = mLogicalDevice.getQueue(physicalDeviceInfo->familyIndexSet.transfer.value(), 0);
        }

        void Core::endInit() {
            //clear mInitInfo, it won't be used anymore
            MX_FREE_POINTER(mInitInfo);
            mInitInfo = nullptr;

            mDynamicLoader.init(static_cast<VkInstance>(mInstance),
                                vkGetInstanceProcAddr,
                                static_cast<VkDevice>(mLogicalDevice),
                                vkGetDeviceProcAddr);

            mSyncObjMgr.init(mLogicalDevice);
        }

        void Core::destroy() {
            mSyncObjMgr.destroy();
            mLogicalDevice.destroy();
            mInstance.destroy(mSurface);
            mInstance.destroy();
            mLogicalDevice = nullptr;
            mInstance = nullptr;
        }

        void Core::setDebugMode(const bool on) {
            mInitInfo->debugMode = on;
            if (mInitInfo->debugMode) {
                mInitInfo->instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                mInitInfo->validationLayers.push_back("VK_LAYER_LUNARG_standard_validation");
            }
        }

        void Core::setTargetWindow(Window * window) {
            mWindow = window;
            unsigned int count;
            SDL_Vulkan_GetInstanceExtensions(window->getWindowPtr(), &count, nullptr);
            std::vector<const char*> extensions(count);
            SDL_Vulkan_GetInstanceExtensions(window->getWindowPtr(), &count, extensions.data());
            mInitInfo->instanceExtensions.insert(mInitInfo->instanceExtensions.end(), extensions.begin(), extensions.end());
        }

        uint32_t Core::getMemoryTypeIndex(const uint32_t type, const vk::MemoryPropertyFlags & properties) const {
            for (uint32_t i = 0; i < mPhysicalDeviceInfo.memoryProperties.memoryTypeCount; ++i) {
                if (type  & (1 << i) && mPhysicalDeviceInfo.memoryProperties.memoryTypes[i].propertyFlags & properties)
                    return i;
            }
            return ~0U;
        }

        bool Core::checkFormatFeatureSupport(const vk::Format format, const vk::ImageTiling tiling, const vk::FormatFeatureFlags features) const {
            auto prop = mPhysicalDeviceInfo.physicalDevice.getFormatProperties(format);

            if (tiling == vk::ImageTiling::eLinear && (prop.linearTilingFeatures & features) == features)
                return true;
            else if (tiling == vk::ImageTiling::eOptimal && (prop.optimalTilingFeatures & features) == features)
                return true;
            else
                return false;
        }

        uint32_t Core::evaluatePhysicalDevice(const PhysicalDeviceInfo & info) {
            // todo complete this in later update
            // divide required extensions into two parts
            // [must supported]: if not score is zero
            // [optinal]:if support one of this part, then ++score
            uint32_t score = 0;
            if (info.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                ++score;

            {
                // when meet one required extension, ++score
                bool found = false;
                for (auto& rqExtension : mInitInfo->deviceExtensions) {
                    found = false;
                    for (const auto& avaliable : info.extensions) {
                        if (strcmp(rqExtension, avaliable.extensionName) == 0) {
                            found = true;
                            break;
                        }
                    }
                    if (found)
                        ++score;
                    else
                        return 0;
                }
            }

            {
                // if device cannot meet the queue flags requirement, score is zero
                bool suit = true;
                if (!info.familyIndexSet.present)
                    suit = false;

                if ((mInitInfo->queueFlags & vk::QueueFlagBits::eGraphics) && (!info.familyIndexSet.graphics))
                    suit = false;

                if ((mInitInfo->queueFlags & vk::QueueFlagBits::eCompute) && (!info.familyIndexSet.compute))
                    suit = false;

                if ((mInitInfo->queueFlags & vk::QueueFlagBits::eTransfer) && (!info.familyIndexSet.transfer))
                    suit = false;

                if (!suit)
                    return 0;
            }

            return score;
        }

        QueueFamilyIndexSet Core::getQueueFamilyIndexSet(const PhysicalDeviceInfo& info) {
            QueueFamilyIndexSet indexSet;
            for (QueueFamilyIndex i = 0; i < info.queueFamilies.size(); ++i) {
                if (info.queueFamilies[i].queueCount == 0) {
                    return indexSet;
                }

                if (!indexSet.graphics && (info.queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics))
                    indexSet.graphics = i;

                if (!indexSet.compute && (info.queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute))
                    indexSet.compute = i;

                if (!indexSet.transfer && (info.queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer))
                    indexSet.transfer = i;

                if (!indexSet.present && info.physicalDevice.getSurfaceSupportKHR(i, mSurface))
                    indexSet.present = i;
            }
            return indexSet;
        }
    }
}

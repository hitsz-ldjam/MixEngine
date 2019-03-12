#pragma once
#ifndef _MX_VK_CORE_H_
#define _MX_VK_CORE_H_


#include "../Mx/MxWindow.h"
#include <SDL2/SDL_vulkan.h>

#include "MxVkDef.h"
#include "MxVkExcept.hpp"
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <optional>


namespace Mix {
    namespace Graphics {
        class Core;

        class GraphicsBase :public Object {
            MX_DECLARE_RTTI;
            MX_DECLARE_NO_CLASS_FACTORY;
        public:
            virtual ~GraphicsBase() = 0 {};
        };


        class Core :public GraphicsBase {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            Core();
            virtual ~Core() {
                destroy();
            }

            void createInstance();
            void pickPhysicalDevice(const std::function<uint32_t(const PhysicalDeviceInfo&)>& marker = nullptr);
            void createLogicalDevice();
            void endInit();
            void destroy();

            const vk::Instance& instance() const { return mInstance; }
            const vk::PhysicalDevice& physicalDevice() const { return mPhysicalDeviceInfo.physicalDevice; }
            const vk::Device& device() const { return mLogicalDevice; }
            const vk::SurfaceKHR& surface() const { return mSurface; }
            Window* window() { return mWindow; }

            void setAppInfo(const std::string& appName, const VersionInt appVer) {
                mAppName = appName;
                mAppVersion = appVer;
            };

            void setDebugMode(const bool on);

            void setValidationLayers(const std::vector<const char*> layers) {
                mInitInfo->validationLayers = layers;
            }

            void setInstanceExtensions(const std::vector<const char*> extensions) {
                mInitInfo->instanceExtensions = extensions;
            }

            void setDeviceExtensions(const std::vector<const char*> extensions) {
                mInitInfo->deviceExtensions = extensions;
            }

            void setQueueFlags(const vk::QueueFlags& flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer) {
                mInitInfo->queueFlags = flags;
            }

            void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& features) {
                mInitInfo->physicalDeviceFeatures = features;
            }

            void setTargetWindow(Window* window);

            const std::vector<PhysicalDeviceInfo>& getAllPhysicalDeviceInfo() const {
                return mAllPhysicalDevices;
            }

            const QueueSet& getQueues() const {
                return mQueueSet;
            }

            const QueueFamilyIndexSet& getQueueFamilyIndices() const {
                return mPhysicalDeviceInfo.familyIndexSet;
            }

            const vk::PhysicalDeviceProperties& getPhysicalDeviceProperties() const {
                return mPhysicalDeviceInfo.properties;
            }

            const vk::PhysicalDeviceFeatures& getPhysicalDeviceFeatures() const {
                return mPhysicalDeviceInfo.features;
            }

            const uint32_t getMemoryTypeIndex(const uint32_t type, const vk::MemoryPropertyFlags& properties) const;

            const vk::DispatchLoaderDynamic& dynamicLoader() const {
                return mDynamicLoader;
            }

            const vk::DispatchLoaderStatic& staticLoader() const {
                return mStaticLoader;
            }

            static std::vector<vk::ExtensionProperties> getSupportedExtensions() {
                return vk::enumerateInstanceExtensionProperties();
            }

            static std::vector<vk::LayerProperties> getSupportedLayers() {
                return vk::enumerateInstanceLayerProperties();
            }
        private:
            Window* mWindow;

            std::string mAppName;
            VersionInt mAppVersion;

            struct InitInfo {

                bool debugMode;
                std::vector<const char*> validationLayers;
                std::vector<const char*> instanceExtensions;
                std::vector<const char*> deviceExtensions;
                vk::QueueFlags queueFlags;
                vk::PhysicalDeviceFeatures physicalDeviceFeatures;
            }*mInitInfo;

            vk::Instance mInstance;
            PhysicalDeviceInfo mPhysicalDeviceInfo;
            vk::Device mLogicalDevice;
            QueueSet mQueueSet;
            vk::SurfaceKHR mSurface;
            vk::DispatchLoaderDynamic mDynamicLoader;
            vk::DispatchLoaderStatic mStaticLoader;


            std::vector<vk::ExtensionProperties> mSupportedExtensions;
            std::vector<vk::LayerProperties> mSupportedLayers;
            std::vector<PhysicalDeviceInfo> mAllPhysicalDevices;

        private:
            uint32_t evaluatePhysicalDevice(const PhysicalDeviceInfo& info);
            QueueFamilyIndexSet getQueueFamilyIndexSet(const PhysicalDeviceInfo& info);
        };


        class GraphicsComponent :public GraphicsBase {
            MX_DECLARE_RTTI;
            MX_DECLARE_NO_CLASS_FACTORY;
        public:
            virtual ~GraphicsComponent() = 0 {};
            GraphicsComponent() { mCore = nullptr; }
            virtual void init(const Core* core) { mCore = core; }
        protected:
            const Core* mCore = nullptr;
        };
    }
}

#endif // !_MX_VK_CORE_H_

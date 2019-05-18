#pragma once
#ifndef _MX_VK_CORE_H_
#define _MX_VK_CORE_H_


#include "../Mx/MxWindow.h"

#include "MxVkDef.h"
#include "MxVkSyncObjMgr.h"
#include <string>
#include <vector>
#include <functional>
#include <SDL2/SDL_vulkan.h>


namespace Mix {
    namespace Graphics {
        class Core final {
        public:
            Core();

            ~Core() {
                destroy();
            }

            void createInstance();
            void pickPhysicalDevice(const std::function<uint32_t(const PhysicalDeviceInfo&)>& marker = nullptr);
            void createLogicalDevice();
            void endInit();
            void destroy();

            const vk::Instance& getInstance() const { return mInstance; }
            const vk::PhysicalDevice& getPhysicalDevice() const { return mPhysicalDeviceInfo.physicalDevice; }
            const vk::Device& getDevice() const { return mLogicalDevice; }
            const vk::SurfaceKHR& getSurface() const { return mSurface; }
            Window* getWindow() const { return mWindow; }

            void setAppInfo(const std::string& _appName, const VersionInt _appVer) {
                mAppName = _appName;
                mAppVersion = _appVer;
            }

            void setDebugMode(bool _on) const;

            void setValidationLayers(const std::vector<const char*>& _layers) const {
                mInitInfo->validationLayers = _layers;
            }

            void setInstanceExtensions(const std::vector<const char*>& _extensions) const {
                mInitInfo->instanceExtensions = _extensions;
            }

            void setDeviceExtensions(const std::vector<const char*>& _extensions) const {
                mInitInfo->deviceExtensions = _extensions;
            }

            void setQueueFlags(
                const vk::QueueFlags& _flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer) const {
                mInitInfo->queueFlags = _flags;
            }

            void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& _features) const {
                mInitInfo->physicalDeviceFeatures = _features;
            }

            void setTargetWindow(Window* _window);

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

            uint32_t getMemoryTypeIndex(uint32_t _type, const vk::MemoryPropertyFlags& _properties) const;

            bool checkFormatFeatureSupport(vk::Format _format, vk::ImageTiling _tiling, const vk::FormatFeatureFlags
                                           & _features) const;

            const vk::DispatchLoaderDynamic& dynamicLoader() const {
                return mDynamicLoader;
            }

            const vk::DispatchLoaderStatic& staticLoader() const {
                return mStaticLoader;
            }

            SyncObjectMgr& getSyncObjMgr() {
                return mSyncObjMgr;
            }

            static std::vector<vk::ExtensionProperties> GetSupportedExtensions() {
                return vk::enumerateInstanceExtensionProperties();
            }

            static std::vector<vk::LayerProperties> GetSupportedLayers() {
                return vk::enumerateInstanceLayerProperties();
            }

        private:
            Window* mWindow;

            std::string mAppName;
            VersionInt mAppVersion;

            struct InitInfo {
                bool debugMode = false;
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

            // classes that are used in other part of program frequently
            SyncObjectMgr mSyncObjMgr;

            uint32_t evaluatePhysicalDevice(const PhysicalDeviceInfo& _info) const;
            QueueFamilyIndexSet getQueueFamilyIndexSet(const PhysicalDeviceInfo& _info) const;
        };


        class GraphicsComponent {
        public:
            virtual ~GraphicsComponent();

            GraphicsComponent() { mCore = nullptr; }

            GraphicsComponent(const GraphicsComponent& _other) = default;

            GraphicsComponent(GraphicsComponent&& _other) noexcept = default;

            GraphicsComponent& operator=(const GraphicsComponent& _other) = default;

            GraphicsComponent& operator=(GraphicsComponent&& _other) noexcept = default;

            void setCore(const std::shared_ptr<Core>& _core) {
                mCore = _core;
            }

        protected:
            std::shared_ptr<Core> mCore;
        };
    }
}

#endif // !_MX_VK_CORE_H_

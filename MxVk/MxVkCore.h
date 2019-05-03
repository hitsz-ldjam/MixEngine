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
                Destroy();
            }

            void CreateInstance();
            void PickPhysicalDevice(const std::function<uint32_t(const PhysicalDeviceInfo&)>& marker = nullptr);
            void CreateLogicalDevice();
            void EndInit();
            void Destroy();

            const vk::Instance& GetInstance() const { return mInstance; }
            const vk::PhysicalDevice& GetPhysicalDevice() const { return mPhysicalDeviceInfo.physicalDevice; }
            const vk::Device& GetDevice() const { return mLogicalDevice; }
            const vk::SurfaceKHR& GetSurface() const { return mSurface; }
            Window* GetWindow() const { return mWindow; }

            void SetAppInfo(const std::string& _appName, const VersionInt _appVer) {
                mAppName = _appName;
                mAppVersion = _appVer;
            }

            void SetDebugMode(bool _on) const;

            void SetValidationLayers(const std::vector<const char*>& _layers) const {
                mInitInfo->validationLayers = _layers;
            }

            void SetInstanceExtensions(const std::vector<const char*>& _extensions) const {
                mInitInfo->instanceExtensions = _extensions;
            }

            void SetDeviceExtensions(const std::vector<const char*>& _extensions) const {
                mInitInfo->deviceExtensions = _extensions;
            }

            void SetQueueFlags(
                const vk::QueueFlags& _flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer) const {
                mInitInfo->queueFlags = _flags;
            }

            void SetPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& _features) const {
                mInitInfo->physicalDeviceFeatures = _features;
            }

            void SetTargetWindow(Window* _window);

            const std::vector<PhysicalDeviceInfo>& GetAllPhysicalDeviceInfo() const {
                return mAllPhysicalDevices;
            }

            const QueueSet& GetQueues() const {
                return mQueueSet;
            }

            const QueueFamilyIndexSet& GetQueueFamilyIndices() const {
                return mPhysicalDeviceInfo.familyIndexSet;
            }

            const vk::PhysicalDeviceProperties& GetPhysicalDeviceProperties() const {
                return mPhysicalDeviceInfo.properties;
            }

            const vk::PhysicalDeviceFeatures& GetPhysicalDeviceFeatures() const {
                return mPhysicalDeviceInfo.features;
            }

            uint32_t GetMemoryTypeIndex(uint32_t _type, const vk::MemoryPropertyFlags& _properties) const;

            bool CheckFormatFeatureSupport(vk::Format _format, vk::ImageTiling _tiling, const vk::FormatFeatureFlags
                                           & _features) const;

            const vk::DispatchLoaderDynamic& DynamicLoader() const {
                return mDynamicLoader;
            }

            const vk::DispatchLoaderStatic& StaticLoader() const {
                return mStaticLoader;
            }

            SyncObjectMgr& GetSyncObjMgr() {
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

            uint32_t EvaluatePhysicalDevice(const PhysicalDeviceInfo& _info) const;
            QueueFamilyIndexSet GetQueueFamilyIndexSet(const PhysicalDeviceInfo& _info);
        };


        class GraphicsComponent {
        public:
            virtual ~GraphicsComponent() = 0 {};

            GraphicsComponent() { mCore = nullptr; }
            virtual void Init(std::shared_ptr<Core>& _core) { mCore = _core; }
        protected:
            std::shared_ptr<Core> mCore;
        };
    }
}

#endif // !_MX_VK_CORE_H_

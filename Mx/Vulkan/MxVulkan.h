#pragma once
#ifndef MX_VK_GRAPHICS_H_
#define MX_VK_GRAPHICS_H_

#include "../Window/MxWindow.h"
#include "../Object/MxObject.h"

#include "Core/MxVkDef.h"
#include "CommandBuffer/MxVkCommanddBufferHandle.h"
#include "../Definitions/MxVersion.h"
#include "Pipeline/MxVkRenderPass.h"
#include "FrameBuffer/MxVkFramebuffer.h"

namespace Mix {
    class Camera;

    namespace Vulkan {
        class Instance;
        class Device;
        class PhysicalDevice;
        class DeviceAllocator;
        class CommandPool;
        class DebugUtils;
        class Swapchain;
        class PipelineMgr;
        class RenderPass;
        class DescriptorPool;
        class DescriptorSetLayout;
        class FrameBuffer;
        class Image;
        class Buffer;
        class DynamicUniformBuffer;
        class ShaderBase;
        class VertexInputManager;

        struct VulkanSettings {
            struct {
                Version appVersion;
                std::string appName;
            }appInfo;
            bool debugMode;
            std::vector<const char*> instanceExts;
            std::vector<const char*> deviceExts;
            std::vector<const char*> validationLayers;
            uint32_t physicalDeviceIndex;
            vk::PhysicalDeviceFeatures enabledFeatures;
        };

        class VulkanAPI {
        public:
            void init();

            static std::vector<vk::ExtensionProperties> GetAllSupportedInstanceExts();

            static std::vector<vk::LayerProperties> GetAllSupportedLayers();

            const std::vector<PhysicalDeviceInfo>& getAllPhysicalDeviceInfo() const { return *mPhysicalDeviceInfos; }

            void setTargetWindow(Window* _window) {
                mWindow = _window;
            }

            void build(const VulkanSettings& _settings);

            const std::shared_ptr<Device>& getLogicalDevice() const {
                return mDevice;
            }

            const std::shared_ptr<PhysicalDevice>& getPhysicalDevice() const {
                return mPhysicalDevice;
            }

            const std::shared_ptr<DeviceAllocator>& getAllocator() const {
                return mAllocator;
            }

            const std::shared_ptr<Swapchain>& getSwapchain() const {
                return mSwapchain;
            }

            const std::shared_ptr<CommandPool>& getTransferCommandPool() const { return mTransferCommandPool; }

            const std::shared_ptr<CommandPool>& getGraphicsCommandPool() const { return mGraphicsCommandPool; }

            const std::shared_ptr<DescriptorPool>& getDescriptorPool() const { return mDescriptorPool; }

            void beginRender();

            void endRender();

            uint32_t getCurrFrame() const { return mCurrFrame; }

            CommandBufferHandle& getCurrDrawCmd()const { return *mCurrCmd; }

            const std::shared_ptr<RenderPass>& getRenderPass() { return mRenderPass; }

            const FrameBuffer& getCurrFrameBuffer() const { return mFrameBuffers[mCurrFrame]; }

            void waitDeviceIdle();

            ~VulkanAPI();

            // Test managers
            VertexInputManager& getVertexInputManager() { return *mVertexInputManager; }

        private:
            void build();
            void createInstance();
            void pickPhysicalDevice();
            void createDevice();
            void createDebugUtils();
            void createDescriptorPool();
            void createSwapchain();
            void createCommandPool();
            void createAllocator();

            void createRenderPass();
            void createFrameBuffer();

            std::shared_ptr<VulkanSettings> mSettings;

            std::shared_ptr<std::vector<PhysicalDeviceInfo>> mPhysicalDeviceInfos;

            Window*								mWindow = nullptr;
            vk::SurfaceKHR						mSurface;
            std::shared_ptr<Instance>			mInstance;
            std::shared_ptr<Device>				mDevice;
            std::shared_ptr<PhysicalDevice>		mPhysicalDevice;
            std::shared_ptr<DebugUtils>         mDebugUtils;
            std::shared_ptr<DeviceAllocator>    mAllocator;
            std::shared_ptr<Swapchain>          mSwapchain;
            std::shared_ptr<DescriptorPool>		mDescriptorPool;

            std::shared_ptr<RenderPass> mRenderPass;
            std::vector<FrameBuffer> mFrameBuffers;
            std::shared_ptr<Image> mDepthStencil;
            vk::ImageView mDepthStencilView;

            std::shared_ptr<CommandPool>		mTransferCommandPool;
            std::shared_ptr<CommandPool>		mGraphicsCommandPool;

            // Test managers
            std::shared_ptr<VertexInputManager> mVertexInputManager;

            std::vector<std::shared_ptr<CommandBufferHandle>> mGraphicsCommandBuffers;

            uint32_t mCurrFrame = 0;
            CommandBufferHandle* mCurrCmd = nullptr;
        };
    }
}

#endif // !MX_VK_GRAPHICS_H_

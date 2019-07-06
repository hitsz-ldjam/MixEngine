#pragma once
#ifndef MX_VK_GRAPHICS_H_
#define MX_VK_GRAPHICS_H_

#include "../Window/MxWindow.h"
#include "../Object/MxObject.h"

#include "../Definitions/MxDefinitions.h"
#include "../Engine/MxModuleBase.h"
#include "Renderer/MxVkRendererHolder.h"
#include "Core/MxVkDef.h"
#include "CommandBuffer/MxVkCommanddBufferHandle.h"

namespace Mix {
	namespace Graphics {
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
		class RendererBase;

		struct VulkanSettings {
			struct {
				VersionInt appVersion;
				std::string appName;
			}appInfo;
			bool debugMode;
			std::vector<const char*> instanceExts;
			std::vector<const char*> deviceExts;
			std::vector<const char*> validationLayers;
			uint32_t physicalDeviceIndex;
			vk::PhysicalDeviceFeatures enabledFeatures;
		};

		class Vulkan :public ModuleBase {
		public:
			void init();

			static std::vector<vk::ExtensionProperties> GetAllSupportedInstanceExts();

			static std::vector<vk::LayerProperties> GetAllSupportedLayers();

			const std::vector<PhysicalDeviceInfo>& getAllPhysicalDeviceInfo() const { return *mPhysicalDeviceInfos; }

			void setTargetWindow(Window* _window) {
				mWindow = _window;
			}

			void build(const VulkanSettings& _settings);

			void build(const VulkanSettings* _settings);

			std::shared_ptr<Device> getLogicalDevice() const {
				return mDevice;
			}

			std::shared_ptr<PhysicalDevice> getPhysicalDevice() const {
				return mPhysicalDevice;
			}

			std::shared_ptr<DeviceAllocator> getAllocator() const {
				return mAllocator;
			}

			std::shared_ptr<Swapchain> getSwapchain() const {
				return mSwapchain;
			}

			std::shared_ptr<CommandPool> getTransferCommandPool() const { return mTransferCommandPool; }

			std::shared_ptr<CommandPool> getGraphicsCommandPool() const { return mGraphicsCommandPool; }

			std::shared_ptr<DescriptorPool> getDescriptorPool() const { return mDescriptorPool; }

			void update();

			void render();

			uint32_t addRenderer(RendererBase* _renderer);

			template<typename _Ty>
			uint32_t addRenderer();

			template<typename _Ty, typename... _Args>
			uint32_t addRenderer(_Args&&... _args);

			RendererBase* getRenderer(const uint32_t _index) const;

			void removeRenderer(const uint32_t _index) const;

			bool hasRenderer(const uint32_t _index) const { return mRendererHolder->hasRenderer(_index); }

			void setActiveRenderer(const uint32_t& _index) { mActiveRenderer = _index; }

			~Vulkan();

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

			// void buildCore();
			// void buildDebugUtils();
			// void buildSwapchain();
			// void buildDepthStencil();
			// void buildRenderPass();
			// void buildDescriptorSetLayout();
			// void buildShaders();
			// void buildPipeline();
			// void buildCommandMgr();
			// void buildFrameBuffers();

			// void buildUniformBuffers();
			// void buildCommandBuffers();
			// void buildDescriptorSets();

			// test
			// build utils to load resources
			// void loadResource();

			// void updateCmdBuffer();
			// void updateUniformBuffer();

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

			std::shared_ptr<CommandPool>		mTransferCommandPool;
			std::shared_ptr<CommandPool>		mGraphicsCommandPool;

			std::vector<CommandBufferHandle>	mGraphicsCommandBuffers;

			std::shared_ptr<RendererHolder>		mRendererHolder;
			uint32_t							mActiveRenderer = 0;

			//std::shared_ptr<ShaderMgr>              mShaderMgr;
			//std::shared_ptr<PipelineMgr>            mPipelineMgr;

			//std::shared_ptr<RenderPass>             mRenderPass;
			//std::shared_ptr<DescriptorPool>         mDescriptorPool;
			//std::map<std::string, std::shared_ptr<DescriptorSetLayout>>    mDescriptorSetLayout;
			//std::shared_ptr<CommandMgr>             mCommandMgr;

			//// std::shared_ptr<ImageMgr>               mImageMgr;
			//// std::shared_ptr<gltf::MeshMgr>          mMeshMgr;

			//std::vector<Framebuffer*>       mFramebuffers;
			//std::shared_ptr<Image>			mDepthStencil;
			//vk::ImageView                   mDepthStencilView;
			//std::vector<vk::CommandBuffer>  mCommandBuffers;
			//std::vector<vk::DescriptorSet>  mDescriptorSets;

			//// todo
			//vk::ImageView texImageView;
			//vk::Sampler sampler;

			//std::vector<std::shared_ptr<Buffer>> cameraUniforms;
			//std::vector<std::shared_ptr<DynamicUniformBuffer>> dynamicUniformBuffers;
		};

		template <typename _Ty>
		uint32_t Vulkan::addRenderer() {
			static_assert(std::is_base_of_v<RendererBase, _Ty>, "A Renderer must be a derived class of RendererBase");

			_Ty* t = new _Ty();
			t->setVulkanPointer(this);
			t->init();
			return mRendererHolder->addRenderer(t);
		}

		template <typename _Ty, typename ... _Args>
		uint32_t Vulkan::addRenderer(_Args&&... _args) {
			static_assert(std::is_base_of_v<RendererBase, _Ty>, "A Renderer must be a derived class of RendererBase");

			_Ty* t = new _Ty(std::forward<_Args>(_args)...);
			t->setVulkanPointer(this);
			t->init();
			return mRendererHolder->addRenderer(t);
		}
	}
}

#endif // !MX_VK_GRAPHICS_H_

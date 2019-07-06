//#pragma once
//#ifndef _MX_VK_CORE_H_
//#define _MX_VK_CORE_H_
//
//
//#include "../../Window/MxWindow.h"
//#include "../../Definitions/MxDefinitions.h"
//
//#include "MxVkDef.h"
//
//#include <string>
//#include <utility>
//#include <vector>
//#include <functional>
//
//
//namespace Mix {
//	namespace Graphics {
//		class Core final {
//		public:
//			Core();
//
//			~Core() {
//				destroy();
//			}
//
//			/**
//			 * @brief Create vk::Instance
//			 */
//			void createInstance();
//
//			/**
//			 * @brief Pick a physical device with specified marker or default marker if _marker is nullptr
//			 * @param _marker Marker that score for each physical device,
//			 * @note _marker is a callable object of type: uint32_t(const PhysicalDeviceInfo&)
//			 */
//			void pickPhysicalDevice(const std::function<uint32_t(const PhysicalDeviceInfo&)>& _marker = nullptr);
//
//			/**
//			 * @brief Create VkDevice
//			 */
//			void createLogicalDevice();
//
//			void endInit();
//
//			void destroy();
//
//			/**
//			 * @brief Get vk::Instance
//			 */
//			const vk::Instance& getInstance() const { return mInstance; }
//
//			/**
//			 * @brief Get vk::PhysicalDevice
//			 */
//			const vk::PhysicalDevice& getPhysicalDevice() const { return mPhysicalDeviceInfo.physicalDevice; }
//
//			/**
//			 * @brief Get vk::Device
//			 */
//			const vk::Device& getDevice() const { return mLogicalDevice; }
//
//			/**
//			 * \brief
//			 * \return
//			 */
//			const vk::SurfaceKHR& getSurface() const { return mSurface; }
//
//			Window* getWindow() const { return mWindow; }
//
//			/**
//			 * \brief Set application name and version
//			 */
//			void setAppInfo(const std::string& _appName, const VersionInt _appVer) {
//				mAppName = _appName;
//				mAppVersion = _appVer;
//			}
//
//			/**
//			 * \brief Switch debug mode
//			 */
//			void setDebugMode(bool _on) const;
//
//			/**
//			 * \brief Set enabled validation layers
//			 */
//			void setValidationLayers(const std::vector<const char*>& _layers) const {
//				mInitInfo->validationLayers = _layers;
//			}
//
//			/**
//			 * \brief Set enabled instance extensions
//			 */
//			void setInstanceExtensions(const std::vector<const char*>& _extensions) const {
//				mInitInfo->instanceExtensions = _extensions;
//			}
//
//			/**
//			 * \brief Set enabled device extensions
//			 */
//			void setDeviceExtensions(const std::vector<const char*>& _extensions) const {
//				mInitInfo->deviceExtensions = _extensions;
//			}
//
//			/**
//			 * \brief Set queue that will be used
//			 */
//			void setQueueFlags(
//				const vk::QueueFlags& _flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer) const {
//				mInitInfo->queueFlags = _flags;
//			}
//
//			/**
//			 * \brief Set physical device features that will be used
//			 */
//			void setPhysicalDeviceFeatures(const vk::PhysicalDeviceFeatures& _features) const {
//				mInitInfo->physicalDeviceFeatures = _features;
//			}
//
//			/**
//			 * \brief Set Window that Vulkan will render on
//			 */
//			void setTargetWindow(Window* _window);
//
//			/**
//			 * \brief Get information of all physical device supporting Vulkan
//			 */
//			const std::vector<PhysicalDeviceInfo>& getAllPhysicalDeviceInfo() const {
//				return mAllPhysicalDevices;
//			}
//
//			const QueueSet& getQueues() const {
//				return mQueueSet;
//			}
//
//			const QueueFamilyIndexSet& getQueueFamilyIndices() const {
//				return mPhysicalDeviceInfo.familyIndexSet;
//			}
//
//			const vk::PhysicalDeviceProperties& getPhysicalDeviceProperties() const {
//				return mPhysicalDeviceInfo.properties;
//			}
//
//			const vk::PhysicalDeviceFeatures& getPhysicalDeviceFeatures() const {
//				return mPhysicalDeviceInfo.features;
//			}
//
//			uint32_t getMemoryTypeIndex(uint32_t _type, const vk::MemoryPropertyFlags& _properties) const;
//
//			bool checkFormatFeatureSupport(vk::Format _format, vk::ImageTiling _tiling, const vk::FormatFeatureFlags
//										   & _features) const;
//
//			const vk::DispatchLoaderDynamic& dynamicLoader() const {
//				return mDynamicLoader;
//			}
//
//			const vk::DispatchLoaderStatic& staticLoader() const {
//				return mStaticLoader;
//			}
//
//			SyncObjectMgr& getSyncObjMgr() {
//				return mSyncObjMgr;
//			}
//
//			/**
//			 * @brief Get all extension properties supported by vk::Instance
//			 */
//			static std::vector<vk::ExtensionProperties> GetSupportedExtensions() {
//				return vk::enumerateInstanceExtensionProperties();
//			}
//
//			/**
//			 * @brief Get all layer properties supported by vk::Instance
//			 */
//			static std::vector<vk::LayerProperties> GetSupportedLayers() {
//				return vk::enumerateInstanceLayerProperties();
//			}
//
//		private:
//			Window* mWindow;
//
//			std::string mAppName;
//			VersionInt mAppVersion;
//
//			struct InitInfo {
//				bool debugMode = false;
//				std::vector<const char*> validationLayers;
//				std::vector<const char*> instanceExtensions;
//				std::vector<const char*> deviceExtensions;
//				vk::QueueFlags queueFlags;
//				vk::PhysicalDeviceFeatures physicalDeviceFeatures;
//			}*mInitInfo;
//
//			vk::Instance mInstance;
//			PhysicalDeviceInfo mPhysicalDeviceInfo;
//			vk::Device mLogicalDevice;
//			QueueSet mQueueSet;
//			vk::SurfaceKHR mSurface;
//			vk::DispatchLoaderDynamic mDynamicLoader;
//			vk::DispatchLoaderStatic mStaticLoader;
//
//
//			std::vector<vk::ExtensionProperties> mSupportedExtensions;
//			std::vector<vk::LayerProperties> mSupportedLayers;
//			std::vector<PhysicalDeviceInfo> mAllPhysicalDevices;
//
//			// classes that are used in other part of program frequently
//			SyncObjectMgr mSyncObjMgr;
//
//			uint32_t evaluatePhysicalDevice(const PhysicalDeviceInfo& _info) const;
//			QueueFamilyIndexSet getQueueFamilyIndexSet(const PhysicalDeviceInfo& _info) const;
//		};
//
//
//		class GraphicsComponent {
//		public:
//			virtual ~GraphicsComponent() = default;
//
//			GraphicsComponent() { mCore = nullptr; }
//
//			GraphicsComponent(std::shared_ptr<Core> _core) :mCore(std::move(_core)) {}
//
//			GraphicsComponent(const GraphicsComponent& _other) = default;
//
//			GraphicsComponent(GraphicsComponent&& _other) noexcept = default;
//
//			GraphicsComponent& operator=(const GraphicsComponent& _other) = default;
//
//			GraphicsComponent& operator=(GraphicsComponent&& _other) noexcept = default;
//
//			void setCore(const std::shared_ptr<Core>& _core) {
//				mCore = _core;
//			}
//
//		protected:
//			std::shared_ptr<Core> mCore;
//		};
//	}
//}
//
//#endif // !_MX_VK_CORE_H_

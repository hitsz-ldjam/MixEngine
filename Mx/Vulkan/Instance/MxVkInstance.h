#pragma once
#ifndef MX_VK_INSTANCE_H_
#define MX_VK_INSTANCE_H_

#include "../../Utils/MxGeneralBase.hpp"
#include <vulkan/vulkan.hpp>

namespace Mix {
	namespace Graphics {
		class Instance :public GeneralBase::NoCopyBase {
		public:
			Instance(const vk::ApplicationInfo& _appInfo,
					 const std::vector<const char*>& _extensions = {},
					 bool _debugOn = false,
					 const std::vector<const char*>& _layers = {});

			Instance(Instance&& _other) noexcept { swap(_other); }

			Instance& operator=(Instance&& _other) noexcept { swap(_other); return *this; }

			void swap(Instance& _other) noexcept;

			const vk::Instance& get() const { return mInstance.get(); }

			const std::vector<const char*>& getEnabledLayers() const { return mEnabledLayers; }

			const std::vector<const char*>& getEnabledExts() const { return mEnabledExts; }

			const std::vector<vk::LayerProperties>& getSupportedLayers() const { return mSupportedLayers; }

			const std::vector<vk::ExtensionProperties>& getSupportedExts() const { return mSupportedExts; }

			static std::vector<vk::ExtensionProperties> GetSupportedExts() {
				return vk::enumerateInstanceExtensionProperties();
			}

			static std::vector<vk::LayerProperties> GetSupportedLayers() {
				return vk::enumerateInstanceLayerProperties();
			}

			static std::vector<const char*> DefaultLayerDebug;

			static std::vector<const char*> DefaultInstanceExtsDebug;

			static std::vector<const char*> DefaultInstanceExts;

		private:
			vk::UniqueInstance mInstance;

			bool mDebugMode = false;
			std::vector<const char*> mEnabledLayers;
			std::vector<const char*> mEnabledExts;

			std::vector<vk::LayerProperties> mSupportedLayers;
			std::vector<vk::ExtensionProperties> mSupportedExts;
		};
	}
}

#endif

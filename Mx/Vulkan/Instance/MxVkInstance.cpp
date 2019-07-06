#include "MxVkInstance.h"

namespace Mix {
	namespace Graphics {
		std::vector<const char*> Instance::DefaultLayerDebug = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		std::vector<const char*> Instance::DefaultInstanceExts = {};

		std::vector<const char*> Instance::DefaultInstanceExtsDebug = {
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};

		Instance::Instance(const vk::ApplicationInfo& _appInfo,
						   const std::vector<const char*>& _extensions,
						   bool _debugOn,
						   const std::vector<const char*>& _layers) {
			vk::InstanceCreateInfo createInfo;
			createInfo.pApplicationInfo = &_appInfo;

			// debug mode
			if (_debugOn) {
				mDebugMode = true;
				createInfo.enabledLayerCount = static_cast<uint32_t>(_layers.size());
				createInfo.ppEnabledLayerNames = _layers.data();
			}
			else {
				createInfo.enabledLayerCount = 0;
				createInfo.ppEnabledLayerNames = nullptr;
			}

			createInfo.enabledExtensionCount = static_cast<uint32_t>(_extensions.size());
			createInfo.ppEnabledExtensionNames = _extensions.data();

			mInstance = vk::createInstanceUnique(createInfo);

			// get information
			mSupportedLayers = GetSupportedLayers();
			mSupportedExts = GetSupportedExts();
		}

		void Instance::swap(Instance& _other) noexcept {
			using std::swap;
			swap(mInstance, _other.mInstance);
			swap(mDebugMode, _other.mDebugMode);
			swap(mEnabledLayers, _other.mEnabledLayers);
			swap(mEnabledExts, _other.mEnabledExts);
			swap(mSupportedLayers, _other.mSupportedLayers);
			swap(mSupportedExts, _other.mSupportedExts);
		}

	}
}

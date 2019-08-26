#include "MxVkPhysicalDevice.h"
#include <map>

namespace Mix {
	namespace Vulkan {
		uint32_t PhysicalDevice::getMemoryTypeIndex(const uint32_t _type, const vk::MemoryPropertyFlags& _properties) const {
			for (uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; ++i) {
				if (_type & (1 << i) && mMemoryProperties.memoryTypes[i].propertyFlags & _properties)
					return i;
			}
			return ~0U;
		}

		bool PhysicalDevice::checkFormatFeatureSupport(const vk::Format _format, const vk::ImageTiling _tiling,
													   const vk::FormatFeatureFlags& _features) const {
			const auto prop = mPhysicalDevice.getFormatProperties(_format);

			if (_tiling == vk::ImageTiling::eLinear && (prop.linearTilingFeatures & _features) == _features)
				return true;
			if (_tiling == vk::ImageTiling::eOptimal && (prop.optimalTilingFeatures & _features) == _features)
				return true;
			return false;
		}

		std::vector<std::shared_ptr<PhysicalDevice>> PhysicalDevice::
			GetAllPhysicalDevice(const std::shared_ptr<Instance>& _instance) {
			auto devices = _instance->get().enumeratePhysicalDevices();
			std::vector<std::shared_ptr<PhysicalDevice>> result;

			for (auto& device : devices)
				result.emplace_back(new PhysicalDevice(_instance, device));
			return result;
		}

		std::shared_ptr<PhysicalDevice> PhysicalDevice::PickPhysicalDevice(const std::shared_ptr<Instance>& _instance,
																		   const MarkerFuncType& _marker) {
			auto devices = _instance->get().enumeratePhysicalDevices();

			if (!devices.empty()) {
				std::multimap<uint32_t, std::shared_ptr<PhysicalDevice>> table;

				for (auto& device : devices) {
					auto p = std::shared_ptr<PhysicalDevice>(new PhysicalDevice(_instance, device));
					table.emplace(_marker ? _marker(p) : DefaultMarker(p), p);
				}

				return table.begin()->second;
			}

			return nullptr;
		}

		PhysicalDevice::PhysicalDevice(const std::shared_ptr<Instance>& _instance,
									   const vk::PhysicalDevice& _physicalDevice)
			:mInstance(_instance) {
			mPhysicalDevice = _physicalDevice;

			mProperties = _physicalDevice.getProperties();
			mFeatures = mPhysicalDevice.getFeatures();
			mExtProperties = mPhysicalDevice.enumerateDeviceExtensionProperties();
			mMemoryProperties = mPhysicalDevice.getMemoryProperties();
			mQueueFamilies = mPhysicalDevice.getQueueFamilyProperties();
		}

		void PhysicalDevice::swap(PhysicalDevice& _other) noexcept {
			using std::swap;
			swap(mInstance, _other.mInstance);
			swap(mPhysicalDevice, _other.mPhysicalDevice);
			swap(mProperties, _other.mProperties);
			swap(mFeatures, _other.mFeatures);
			swap(mExtProperties, _other.mExtProperties);
			swap(mMemoryProperties, _other.mMemoryProperties);
			swap(mQueueFamilies, _other.mQueueFamilies);
		}

		uint32_t PhysicalDevice::DefaultMarker(const std::shared_ptr<PhysicalDevice>& _device) {
			return _device->getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
		}
	}
}

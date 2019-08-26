#pragma once
#ifndef MX_VK_PHYSICAL_DEVICE_H_
#define MX_VK_PHYSICAL_DEVICE_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "../Instance/MxVkInstance.h"
#include <functional>

namespace Mix {
	namespace Vulkan {
		class Instance;

		class PhysicalDevice :public GeneralBase::NoCopyBase {
		public:
			PhysicalDevice(const std::shared_ptr<Instance>& _instance,
						   const vk::PhysicalDevice& _physicalDevice);

			PhysicalDevice(PhysicalDevice&& _other) noexcept { swap(_other); }

			PhysicalDevice& operator=(PhysicalDevice&& _other) noexcept { swap(_other); return *this; }

			void swap(PhysicalDevice& _other) noexcept;

			using MarkerFuncType = std::function<uint32_t(const std::shared_ptr<PhysicalDevice>&)>;

			const vk::PhysicalDevice& get() const { return mPhysicalDevice; }

			std::shared_ptr<Instance> getInstance() const { return mInstance; }

			const vk::PhysicalDeviceProperties& getProperties() const { return mProperties; }

			const vk::PhysicalDeviceFeatures& getFeatures() const { return mFeatures; }

			const std::vector<vk::ExtensionProperties>& getExtProperties() const { return mExtProperties; }

			const vk::PhysicalDeviceMemoryProperties& getMemoryProperties() const { return mMemoryProperties; };

			const std::vector<vk::QueueFamilyProperties>& getQueueFamilyProperties() const { return mQueueFamilies; }

			uint32_t getMemoryTypeIndex(const uint32_t _type, const vk::MemoryPropertyFlags& _properties) const;

			bool checkFormatFeatureSupport(const vk::Format _format, const vk::ImageTiling _tiling,
										   const vk::FormatFeatureFlags& _features) const;

			static std::vector<std::shared_ptr<PhysicalDevice>> GetAllPhysicalDevice(const std::shared_ptr<Instance>& _instance);

			static std::shared_ptr<PhysicalDevice> PickPhysicalDevice(const std::shared_ptr<Instance>& _instance,
																	  const MarkerFuncType& _marker = nullptr);

		private:
			std::shared_ptr<Instance> mInstance;

			vk::PhysicalDevice mPhysicalDevice{};
			vk::PhysicalDeviceProperties mProperties{};
			vk::PhysicalDeviceFeatures mFeatures{};
			std::vector<vk::ExtensionProperties> mExtProperties;
			vk::PhysicalDeviceMemoryProperties mMemoryProperties{};
			std::vector<vk::QueueFamilyProperties> mQueueFamilies;

			static uint32_t DefaultMarker(const std::shared_ptr<PhysicalDevice>& _device);
		};
	}
}

#endif

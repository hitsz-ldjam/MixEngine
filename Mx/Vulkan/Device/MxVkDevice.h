#pragma once
#ifndef MX_VK_DEVICE_H_
#define MX_VK_DEVICE_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "MxVkPhysicalDevice.h"
#include "../Core/MxVkDef.h"

namespace Mix {
	namespace Graphics {
		class Instance;
		class PhysicalDevice;

		class Device :public GeneralBase::NoCopyBase {
		public:
			~Device();

			explicit Device(const std::shared_ptr<PhysicalDevice>& _physicalDevice,
							std::optional<vk::SurfaceKHR> _surface = std::nullopt,
							const vk::PhysicalDeviceFeatures* _enabledFeatures = nullptr,
							const std::vector<const char*>& _enabledExts = {},
							const std::vector<const char*>& _enabledLayers = {},
							const vk::QueueFlags& _requiredQueue = {});

			Device(Device&& _other) noexcept { swap(_other); }

			Device& operator=(Device&& _other) noexcept { swap(_other); return *this; }

			void swap(Device& _other) noexcept;

			const vk::Device& get() const { return mDevice; }

			std::shared_ptr<PhysicalDevice> getPhysicalDevice() const { return mPhysicalDevice; }

			std::optional<vk::SurfaceKHR> getSurface() const { return mSurface; }

			const QueueSet& getQueueSet() const { return mQueueSet; }

			const QueueFamilyIndexSet& getQueueFamilyIndexSet() const { return mQueueFamilyIndexSet; }

			const vk::DispatchLoaderDynamic& getDynamicLoader() const { return mDynamicLoader; }

			const vk::DispatchLoaderStatic& getStaticLoader() const { return mStaticLoader; }

		private:
			vk::Device mDevice;

			std::shared_ptr<PhysicalDevice> mPhysicalDevice;
			std::optional<vk::SurfaceKHR> mSurface;
			vk::DispatchLoaderDynamic mDynamicLoader;
			vk::DispatchLoaderStatic mStaticLoader;

			vk::PhysicalDeviceFeatures mEnabledFeatures;
			std::vector<std::string> mEnabledExts;
			std::vector<std::string> mEnabledLayers;

			QueueFamilyIndexSet mQueueFamilyIndexSet;
			QueueSet mQueueSet;

			QueueFamilyIndexSet getQueueFamilyIndexSet(const PhysicalDevice& _physicalDevice,
													   const vk::QueueFlags& _requiredQueue);
		};
	}
}

#endif

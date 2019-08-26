#include "MxVkDevice.h"
#include <map>

namespace Mix {
	namespace Vulkan {
		Device::Device(const std::shared_ptr<PhysicalDevice>& _physicalDevice,
		               std::optional<vk::SurfaceKHR> _surface,
		               const vk::PhysicalDeviceFeatures* _enabledFeatures,
		               const std::vector<const char*>& _enabledExts,
		               const std::vector<const char*>& _enabledLayers,
		               const vk::QueueFlags& _requiredQueue)
			: mPhysicalDevice(_physicalDevice),
			  mSurface(_surface) {
			mQueueFamilyIndexSet = getQueueFamilyIndexSet(*mPhysicalDevice, _requiredQueue);

			std::map<uint32_t, float> uniqueQueueFamilies;

			if (mQueueFamilyIndexSet.graphics)
				uniqueQueueFamilies[mQueueFamilyIndexSet.graphics.value()] = 1.0f;
			if (mQueueFamilyIndexSet.compute)
				uniqueQueueFamilies[mQueueFamilyIndexSet.compute.value()] = 1.0f;
			if (mQueueFamilyIndexSet.transfer)
				uniqueQueueFamilies[mQueueFamilyIndexSet.transfer.value()] = 1.0f;
			if (mQueueFamilyIndexSet.present)
				uniqueQueueFamilies[mQueueFamilyIndexSet.present.value()] = 1.0f;

			std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
			queueCreateInfos.reserve(4);
			for (const auto& pair : uniqueQueueFamilies) {
				queueCreateInfos.emplace_back();
				queueCreateInfos.back().queueFamilyIndex = pair.first;
				queueCreateInfos.back().pQueuePriorities = &pair.second;
				queueCreateInfos.back().queueCount       = 1;
			}

			vk::DeviceCreateInfo createInfo;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			createInfo.pQueueCreateInfos    = queueCreateInfos.data();

			createInfo.pEnabledFeatures = _enabledFeatures;

			createInfo.enabledExtensionCount   = static_cast<uint32_t>(_enabledExts.size());
			createInfo.ppEnabledExtensionNames = _enabledExts.data();

			createInfo.enabledLayerCount   = static_cast<uint32_t>(_enabledLayers.size());
			createInfo.ppEnabledLayerNames = _enabledLayers.data();

			mDevice = mPhysicalDevice->get().createDevice(createInfo);

			if (_surface)
				mQueueSet.present = mDevice.getQueue(mQueueFamilyIndexSet.present.value(), 0);
			if (_requiredQueue & vk::QueueFlagBits::eGraphics)
				mQueueSet.graphics = mDevice.getQueue(mQueueFamilyIndexSet.graphics.value(), 0);
			if (_requiredQueue & vk::QueueFlagBits::eCompute)
				mQueueSet.compute = mDevice.getQueue(mQueueFamilyIndexSet.compute.value(), 0);
			if (_requiredQueue & vk::QueueFlagBits::eTransfer)
				mQueueSet.transfer = mDevice.getQueue(mQueueFamilyIndexSet.transfer.value(), 0);

			mEnabledFeatures = *_enabledFeatures;
			mEnabledLayers.assign(_enabledLayers.begin(), _enabledLayers.end());
			mEnabledExts.assign(_enabledExts.begin(), _enabledExts.end());

			mDynamicLoader.init(static_cast<VkInstance>(mPhysicalDevice->getInstance()->get()),
								vkGetInstanceProcAddr,
								static_cast<VkDevice>(mDevice),
								vkGetDeviceProcAddr);
		}

		void Device::swap(Device& _other) noexcept {
			using std::swap;
			swap(mDevice, _other.mDevice);
			swap(mPhysicalDevice, _other.mPhysicalDevice);
			swap(mSurface, _other.mSurface);
			swap(mDynamicLoader, _other.mDynamicLoader);
			swap(mStaticLoader, _other.mStaticLoader);
			swap(mEnabledFeatures, _other.mEnabledFeatures);
			swap(mEnabledExts, _other.mEnabledExts);
			swap(mEnabledLayers, _other.mEnabledLayers);
			swap(mQueueFamilyIndexSet, _other.mQueueFamilyIndexSet);
			swap(mQueueSet, _other.mQueueSet);
		}

		QueueFamilyIndexSet Device::getQueueFamilyIndexSet(const PhysicalDevice& _physicalDevice,
		                                                   const vk::QueueFlags& _requiredQueue) {
			QueueFamilyIndexSet indexSet;
			auto& families = _physicalDevice.getQueueFamilyProperties();

			for (QueueFamilyIndex i = 0; i < families.size(); ++i) {
				if (families[i].queueCount == 0)
					continue;

				if (_requiredQueue & families[i].queueFlags & vk::QueueFlagBits::eGraphics &&
				    !indexSet.graphics)
					indexSet.graphics = i;

				if (_requiredQueue & families[i].queueFlags & vk::QueueFlagBits::eCompute &&
				    !indexSet.compute)
					indexSet.compute = i;

				if (_requiredQueue & families[i].queueFlags & vk::QueueFlagBits::eTransfer &&
				    !indexSet.transfer)
					indexSet.transfer = i;

				if (mSurface &&
				    !indexSet.present &&
				    _physicalDevice.get().getSurfaceSupportKHR(i, mSurface.value()))
					indexSet.present = i;
			}
			return indexSet;
		}

		Device::~Device() {
			if (mDevice)
				mDevice.destroy();
		}
	}
}

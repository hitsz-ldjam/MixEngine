#include "MxVkManager.h"

namespace Mix
{
	void MxVkManager::createInstance(const InitializeInfo & info)
	{
		if (info.debugMode && !checkValidationLayerSupport(info))
		{
			throw std::runtime_error("Error : Validation layers required, but not available!");
		}

		//vulkan应用信息
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = info.instance.appInfo.appName.c_str();
		appInfo.applicationVersion = info.instance.appInfo.appVersion;
		appInfo.pEngineName = info.instance.appInfo.engineName.c_str();
		appInfo.engineVersion = info.instance.appInfo.engineVersion;
		appInfo.apiVersion = VK_API_VERSION_1_1;

		//实例创建信息
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//验证层
		if (info.debugMode)
		{
			createInfo.ppEnabledLayerNames = info.instance.validationLayers.data();
			createInfo.enabledLayerCount = static_cast<uint32_t>(info.instance.validationLayers.size());
		} else
		{
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.enabledLayerCount = 0;
		}

		//需要的扩展
		std::vector<const char*> extensions(info.instance.extensions);
		if (info.present)
		{
			unsigned int count;
			SDL_Vulkan_GetInstanceExtensions(info.window, &count, nullptr);
			extensions.resize(count);
			SDL_Vulkan_GetInstanceExtensions(info.window, &count, extensions.data());
		}
		if (info.debugMode)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.insert(extensions.end(), info.instance.extensions.cbegin(), info.instance.extensions.cend());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

		//被支持的扩展
		mInstance.extensionSupported = getInstanceExtensions();

		//创建实例
		MX_VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &mInstance.instance));
	}

	void MxVkManager::createSurface(const InitializeInfo & info)
	{
		if (SDL_Vulkan_CreateSurface(info.window, mInstance.instance, &mSurface) != SDL_TRUE)
		{
			throw std::runtime_error("Error : Failed to create window surface!");
		}
	}

	void MxVkManager::pickPhysicalDevice(const InitializeInfo & info)
	{
		uint32_t deviceCount = 0;
		//枚举物理设备
		vkEnumeratePhysicalDevices(mInstance.instance, &deviceCount, nullptr);
		if (deviceCount < 1)
			throw std::runtime_error("Error : Failed to find device with Vulkan support!");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(mInstance.instance, &deviceCount, devices.data());

		for (auto& device : devices)
		{
			if (isDeviceSuitable(info, device))
			{
				mDevice.physicalDevice = device;
				vkGetPhysicalDeviceProperties(mDevice.physicalDevice, &mDevice.properties);
				vkGetPhysicalDeviceFeatures(mDevice.physicalDevice, &mDevice.features);
				mDevice.extensionSupported = getDeviceExtensions(mDevice.physicalDevice);
				findQueueFamilies(info, mDevice.physicalDevice, &mQueueFamilyIndices);
				break;
			}
		}

		if (mDevice.physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Error : Failed to find suitable device!");
	}

	void MxVkManager::createLogicalDevice(const InitializeInfo & info)
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::map<uint32_t, float> uniqueQueueFamilies;

		if (info.device.physical.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			uniqueQueueFamilies.insert({ mQueueFamilyIndices.graphics, info.device.physical.queuePrioriy.graphics });
		if (info.device.physical.queueFlags & VK_QUEUE_COMPUTE_BIT)
			uniqueQueueFamilies.insert({ mQueueFamilyIndices.compute, info.device.physical.queuePrioriy.compute });
		if (info.present)
			uniqueQueueFamilies.insert({ mQueueFamilyIndices.present, info.device.physical.queuePrioriy.present });

		for (const auto& queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily.first;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queueFamily.second;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = info.device.physical.enabledFeatures;

		//创建逻辑设备
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.ppEnabledExtensionNames = info.device.physical.extensions.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(info.device.physical.extensions.size());

		if (info.debugMode)
		{
			deviceCreateInfo.ppEnabledLayerNames = info.instance.validationLayers.data();
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(info.instance.validationLayers.size());
		} else
		{
			deviceCreateInfo.ppEnabledLayerNames = nullptr;
			deviceCreateInfo.enabledLayerCount = 0;
		}

		//创建设备
		MX_VK_CHECK_RESULT(vkCreateDevice(mDevice.physicalDevice, &deviceCreateInfo, nullptr, &mDevice.logicalDevice));

		if (info.device.physical.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			vkGetDeviceQueue(mDevice.logicalDevice, mQueueFamilyIndices.graphics, 0, &mQueue.graphics);
		if (info.device.physical.queueFlags & VK_QUEUE_COMPUTE_BIT)
			vkGetDeviceQueue(mDevice.logicalDevice, mQueueFamilyIndices.compute, 0, &mQueue.compute);
		if (info.present)
			vkGetDeviceQueue(mDevice.logicalDevice, mQueueFamilyIndices.present, 0, &(mQueue.present));

	}

	void MxVkManager::getDeviceInfo()
	{
		//physical device memory properties
		vkGetPhysicalDeviceMemoryProperties(mDevice.physicalDevice, &mDevice.memoryProperties);
	}

	bool MxVkManager::checkValidationLayerSupport(const InitializeInfo & info)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayer(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayer.data());

		for (const auto& layerName : info.instance.validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayer)
			{
				if (strcmp(layerProperties.layerName, layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
				return false;
		}
		return true;
	}

	bool MxVkManager::isDeviceSuitable(const InitializeInfo & info, const VkPhysicalDevice & device)
	{
		//获取物理设备特性
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		bool suit = deviceProperties.deviceType == info.device.physical.type && deviceFeatures.geometryShader;
		if (!suit) return false;
		suit = checkExtensions(info, device);
		if (!suit) return false;
		suit = findQueueFamilies(info, device, nullptr);
		if (!suit) return false;
		return true;
	}

	bool MxVkManager::checkExtensions(const InitializeInfo & info, const VkPhysicalDevice & device)
	{
		std::vector<VkExtensionProperties> extensions = getDeviceExtensions(device);
		bool found = false;
		for (const auto& rqExtension : info.device.physical.extensions)
		{
			found = false;
			for (const auto& avaliable : extensions)
			{
				if (strcmp(rqExtension, avaliable.extensionName) == 0)
				{
					found = true;
					break;
				}
			}
			if (!found)
				break;
		}

		if (found)
		{
			mInstance.extensionSupported = std::move(extensions);
			return true;
		} else
			return false;
	}
	bool MxVkManager::findQueueFamilies(const InitializeInfo & info, VkPhysicalDevice device, MxVkQueueFamilyIndices* indices)
	{
		//获取物理设备队列簇特性
		MxVkQueueFamilyIndices tempIndices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		uint32_t i = 0;
		for (auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount < 1)
				continue;
			//队列是否支持graphics
			if (info.device.physical.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					tempIndices.graphics = i;
				} else
				{
					++i;
					continue;
				}
			}

			//队列是否支持compute
			if (info.device.physical.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					tempIndices.compute = i;
				} else
				{
					++i;
					continue;
				}
			}

			//队列是否支持present
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
			if (info.present)
			{
				if (presentSupport)
				{
					tempIndices.present = i;
				} else
				{
					++i;
					continue;
				}
			}

			if (indices)
				*indices = tempIndices;
			return true;
		}
		return false;
	}

	std::vector<VkExtensionProperties> MxVkManager::getInstanceExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		return extensions;
	}

	std::vector<VkExtensionProperties> MxVkManager::getDeviceExtensions(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
		return extensions;
	}

	MxVkManager::MxVkManager() :mIsReady(false)
	{
	}

	void MxVkManager::initialize(const InitializeInfo & info)
	{
		if (mIsReady)
			destroy();
		createInstance(info);
		createSurface(info);
		pickPhysicalDevice(info);
		createLogicalDevice(info);

		getDeviceInfo();
		mIsReady = true;
	}

	void MxVkManager::destroy()
	{
		if (!mIsReady)
			return;
		vkDestroyDevice(mDevice.logicalDevice, nullptr);
		vkDestroySurfaceKHR(mInstance.instance, mSurface, nullptr);
		vkDestroyInstance(mInstance.instance, nullptr);

		mIsReady = false;

		mInstance.instance = VK_NULL_HANDLE;
		mInstance.extensionSupported.clear();

		mDevice.logicalDevice = VK_NULL_HANDLE;
		mDevice.physicalDevice = VK_NULL_HANDLE;

		mQueueFamilyIndices.compute = VK_NULL_HANDLE;
		mQueueFamilyIndices.graphics = VK_NULL_HANDLE;
		mQueueFamilyIndices.present = VK_NULL_HANDLE;
	}

	MxVkManager::InitializeInfo * MxVkManager::getEmptyInitInfo() const
	{
		InitializeInfo* temp = nullptr;
		try
		{
			temp = new InitializeInfo;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return nullptr;
		}
		return temp;
	}

	const uint32_t MxVkManager::getMemoryTypeIndex(const uint32_t type, const VkMemoryPropertyFlags properties) const
	{
		for (uint32_t i = 0; i < mDevice.memoryProperties.memoryTypeCount; ++i)
		{
			if (type  & (1 << i) && mDevice.memoryProperties.memoryTypes[i].propertyFlags & properties)
				return i;
		}
		return ~0U;
	}


	//MxVkDebug * MxVkManager::createVulkanDebug() const
	//{
	//	MxVkDebug* temp = nullptr;
	//	try
	//	{
	//		temp = new MxVkDebug;
	//	}
	//	catch (const std::exception& e)
	//	{
	//		std::cerr << e.what() << std::endl;
	//		return nullptr;
	//	}
	//	temp->setup(mInstance.instance);
	//	return temp;
	//}

}
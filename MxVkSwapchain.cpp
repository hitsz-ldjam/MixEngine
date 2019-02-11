#include "MxVkSwapchain.h"

namespace Mix
{
	MxVkSwapchain::MxVkSwapchain()
	{
	}

	MxVkSwapchain::SwapchainSupportDetails MxVkSwapchain::getSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		{//获取交换链细节信息
			SwapchainSupportDetails details;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}
	}

	bool MxVkSwapchain::chooseFormat(const std::vector<VkSurfaceFormatKHR>& rqFormats, VkSurfaceFormatKHR& format)
	{
		if (mSwapchainSupportDetails.formats.size() == 1 && mSwapchainSupportDetails.formats[0].format == VK_FORMAT_UNDEFINED)
		{
			format = rqFormats[0];
			return true;
		}

		for (const auto& rqFormat : rqFormats)
		{
			for (const auto& support : mSwapchainSupportDetails.formats)
			{
				if (rqFormat.format == support.format && rqFormat.colorSpace == support.colorSpace)
				{
					format = support;
					return true;
				}
			}
		}
		return false;
	}

	bool MxVkSwapchain::choosePresentMode(const VkPresentModeKHR rqPresentMode, VkPresentModeKHR & presentMode)
	{
		for (const auto& support : mSwapchainSupportDetails.presentModes)
		{
			if (rqPresentMode == support)
			{
				presentMode = support;
				return true;
			}
		}
		presentMode = VK_PRESENT_MODE_FIFO_KHR;
		return false;
	}

	void MxVkSwapchain::createSwapchainImageView()
	{
		mSwapChainImageViews.resize(mSwapChainImages.size());
		for (uint32_t i = 0; i < mSwapChainImages.size(); ++i)
		{
			mSwapChainImageViews[i] = MxVkImage::createImageView2D(mManager,
																	   mSwapChainImages[i],
																	   mCurrFormat.format,
																	   VK_IMAGE_ASPECT_COLOR_BIT,
																	   0, 1, 0, 1);
		}
	}

	VkExtent2D MxVkSwapchain::chooseExtent(const VkExtent2D & rqExtent)
	{
		if (mSwapchainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return mSwapchainSupportDetails.capabilities.currentExtent;
		} else
		{
			VkExtent2D actualExtent = { static_cast<uint32_t>(rqExtent.width), static_cast<uint32_t>(rqExtent.height) };
			actualExtent.width = std::max(mSwapchainSupportDetails.capabilities.minImageExtent.width,
										  std::min(mSwapchainSupportDetails.capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(mSwapchainSupportDetails.capabilities.minImageExtent.height,
										   std::min(mSwapchainSupportDetails.capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

	bool MxVkSwapchain::setup(const MxVkManager* manager)
	{
		if (mIsReady)
			destroy();
		mManager = manager;
		mIsReady = true;
		mSwapchainSupportDetails = getSwapChainSupport(mManager->getPhysicalDevice(), mManager->getSurface());
		return true;
	}

	std::vector<VkSurfaceFormatKHR> MxVkSwapchain::getSupportFormat() const
	{
		if (mIsReady)
			return mSwapchainSupportDetails.formats;
		else
			return std::vector<VkSurfaceFormatKHR>();
	}

	std::vector<VkPresentModeKHR> MxVkSwapchain::getSupportPresentMode() const
	{
		if (mIsReady)
			return mSwapchainSupportDetails.presentModes;
		else
			return std::vector<VkPresentModeKHR>();
	}

	bool MxVkSwapchain::createSwapchain(const std::vector<VkSurfaceFormatKHR>& rqFormats, VkPresentModeKHR rqPresentMode, VkExtent2D rqExtent)
	{
		if (!mIsReady)
			return false;

		//交换链surface格式
		VkSurfaceFormatKHR format;
		if (!chooseFormat(rqFormats, format))
			return false;

		//交换链呈现模式
		VkPresentModeKHR presentMode;
		if (!choosePresentMode(rqPresentMode, presentMode))
		{
			return false;
		}
		//交换链
		VkExtent2D extent = chooseExtent(rqExtent);

		//交换链中image的数量
		uint32_t imageCount = mSwapchainSupportDetails.capabilities.minImageCount;
		if (imageCount > 0 && imageCount > mSwapchainSupportDetails.capabilities.maxImageCount)
			imageCount = mSwapchainSupportDetails.capabilities.maxImageCount;

		//创建交换链
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mManager->getSurface();
		createInfo.presentMode = presentMode;
		createInfo.minImageCount = imageCount;
		//格式
		createInfo.imageFormat = format.format;
		//色彩空间
		createInfo.imageColorSpace = format.colorSpace;
		//图像大小
		createInfo.imageExtent = extent;
		//每一个image包含的层数
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //image的用途

		uint32_t queueFamilyIndices[] = { mManager->getQueueFamilyIndices().graphics,
										  mManager->getQueueFamilyIndices().present };
		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{//如果graphics簇与present簇不同
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //共有
			//如果不同，通过queueFamilyIndices来区分不同的队列簇
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
			createInfo.queueFamilyIndexCount = 2;
		} else
		{//如果graphics簇与present簇相同
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //独占
			//独占则不需要考虑一下信息
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		//对交换链中的image所做的预变换
		createInfo.preTransform = mSwapchainSupportDetails.capabilities.currentTransform;
		//确定在与窗口系统中的其他窗口混合是是否使用alpha通道
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		//当窗口被其他窗口遮挡时，是否裁剪掉被遮挡的部分
		createInfo.clipped = true;
		//当交换链发生改变时，填入上一个交换链
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		//创建交换链
		MX_VK_CHECK_RESULT(vkCreateSwapchainKHR(mManager->getDevice(), &createInfo, nullptr, &mSwapchain));
		//获取交换链中的image
		vkGetSwapchainImagesKHR(mManager->getDevice(), mSwapchain, &imageCount, nullptr);
		mSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mManager->getDevice(), mSwapchain, &imageCount, mSwapChainImages.data());
		//保存交换链的格式和大小
		mCurrFormat = format;
		mCurrExtent = extent;
		mCurrPresentMode = presentMode;
		createSwapchainImageView();
		return true;
	}

	void MxVkSwapchain::destroy()
	{
		if (!mIsReady)
			return;
		if (mSwapchain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mManager->getDevice(), mSwapchain, nullptr);
			mSwapchain = VK_NULL_HANDLE;
		}
		for (const auto& view : mSwapChainImageViews)
		{
			vkDestroyImageView(mManager->getDevice(), view, nullptr);
		}
		mSwapChainImages.clear();
		mSwapChainImageViews.clear();
		mSwapchainSupportDetails.formats.clear();
		mSwapchainSupportDetails.presentModes.clear();
		mManager = nullptr;
	}

	MxVkSwapchain::~MxVkSwapchain()
	{
		destroy();
	}
}
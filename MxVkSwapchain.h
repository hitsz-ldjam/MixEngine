#pragma once
#ifndef _MX_VK_SWAPCHAIN_H_
#define _MX_VK_SWAPCHAIN_H_

#include"MxVkImage.h"
#include"MxVkComponent.h"

#include<vector>
#include<algorithm>

namespace Mix
{
	class MxVkSwapchain:MxVkComponent
	{
	private:
		VkSwapchainKHR mSwapchain;
		VkSurfaceFormatKHR mCurrFormat;
		VkPresentModeKHR mCurrPresentMode;
		VkExtent2D mCurrExtent;

		std::vector<VkImage> mSwapChainImages;
		std::vector<VkImageView> mSwapChainImageViews;

		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		}mSwapchainSupportDetails;

		SwapchainSupportDetails getSwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		bool chooseFormat(const std::vector<VkSurfaceFormatKHR>& rqFormats, VkSurfaceFormatKHR& format);
		bool choosePresentMode(const VkPresentModeKHR rqPresentMode, VkPresentModeKHR& presentMode);
		void createSwapchainImageView();
		VkExtent2D chooseExtent(const VkExtent2D& rqExtent);

	public:
		MxVkSwapchain();
		bool setup(const MxVkManager* manager);
		std::vector<VkSurfaceFormatKHR> getSupportFormat() const;
		std::vector<VkPresentModeKHR> getSupportPresentMode() const;
		bool createSwapchain(const std::vector<VkSurfaceFormatKHR>& rqFormats,
							 VkPresentModeKHR rqPresentMode,
							 VkExtent2D rqExtent);
		VkSwapchainKHR getSwapchain()const { return mSwapchain; };
		const VkSurfaceFormatKHR& getCurrFormat() const { return mCurrFormat; };
		VkPresentModeKHR getCurrPresentMode() const { return mCurrPresentMode; };
		const VkExtent2D& getCurrExtent() const { return mCurrExtent; };
		uint32_t getImageCount() const { return mSwapChainImages.size(); };
		const std::vector<VkImageView> getImageViews() const { return mSwapChainImageViews; };
		void destroy();
		~MxVkSwapchain();
	};
}

#endif
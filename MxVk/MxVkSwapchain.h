#pragma once
#ifndef _MX_VK_SWAPCHAIN_H_
#define _MX_VK_SWAPCHAIN_H_

#include "MxVkExcept.hpp"
#include "MxVkCore.h"
#include "MxVkImage.h"
#include "MxVkSyncObjMgr.h"
#include <numeric>


namespace Mix {
    namespace Graphics {
        class Swapchain :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~Swapchain() {
                destroy();
            }

            virtual void init(std::shared_ptr<Core>& core) override;

            void setImageCount(const uint32_t count) {
                mImageCount = mSupportDetails.capabilities.minImageCount < count ? count : mSupportDetails.capabilities.minImageCount;
                mImageCount = mImageCount < mSupportDetails.capabilities.maxImageCount ? mImageCount : mSupportDetails.capabilities.maxImageCount;
            }

            void destroy();

            void create(const std::vector<vk::SurfaceFormatKHR>& rqFormats,

                        const std::vector<vk::PresentModeKHR>& rqPresentMode,
                        const vk::Extent2D& rqExtent);

            const std::vector<vk::SurfaceFormatKHR>& supportedFormat() const {
                return mSupportDetails.formats;
            }

            const std::vector<vk::PresentModeKHR>& supportedPresentMode() const {
                return mSupportDetails.presentModes;
            }

            const vk::SwapchainKHR& get() const { return mSwapchain; }

            const vk::SurfaceFormatKHR& surfaceFormat() const { return mSurfaceFormat; }

            vk::PresentModeKHR presentMode() const { return mPresentMode; }

            const vk::Extent2D& extent() const { return mExtent; }

            uint32_t imageCount() const { return mImageCount; }

            size_t currentFrame() const { return mCurrFrame; }

            const std::vector<vk::ImageView>& imageViews() const { return mImageViews; }

            void present(vk::CommandBuffer& cmdBuffer);

        private:
            vk::SwapchainKHR mSwapchain;
            vk::SurfaceFormatKHR mSurfaceFormat;
            vk::PresentModeKHR mPresentMode;
            vk::Extent2D mExtent;
            size_t mCurrFrame = 0;

            uint32_t mImageCount = 2;
            std::vector<vk::Image> mImages;
            std::vector<vk::ImageView> mImageViews;

            std::vector<Semaphore> mImageAvlSph;
            std::vector<Semaphore> mRenderFinishedSph;
            std::vector<Fence> mInFlightFences;

            SwapchainSupportDetails mSupportDetails;

            vk::Extent2D chooseExtent(const vk::Extent2D& rqExtent);
            bool choosePresentMode(const std::vector<vk::PresentModeKHR>& rqPresentModes, vk::PresentModeKHR& presentMode);
            bool chooseFormat(const std::vector<vk::SurfaceFormatKHR>& rqFormats, VkSurfaceFormatKHR& format);
            void createSwapchainImageView();
        };
    }
}
#endif // !_MX_VK_SWAPCHAIN_H_

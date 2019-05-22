#pragma once
#ifndef MX_VK_SWAPCHAIN_H_
#define MX_VK_SWAPCHAIN_H_

#include "../Core/MxVkCore.h"
#include "../Image/MxVkImage.h"
#include "../Core/MxVkSyncObjMgr.h"

namespace Mix {
    namespace Graphics {
        class Swapchain :public GraphicsComponent {
        public:
            ~Swapchain() {
                destroy();
            }

            void init(const std::shared_ptr<Core>& _core);

            void setImageCount(const uint32_t _count) {
                mImageCount = mSupportDetails.capabilities.minImageCount < _count ? _count : mSupportDetails.capabilities.minImageCount;
                mImageCount = mImageCount < mSupportDetails.capabilities.maxImageCount ? mImageCount : mSupportDetails.capabilities.maxImageCount;
            }

            void destroy();

            void create(const std::vector<vk::SurfaceFormatKHR>& _rqFormats,
                        const std::vector<vk::PresentModeKHR>& _rqPresentMode,
                        const vk::Extent2D& _rqExtent);

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

            void present(vk::CommandBuffer& _cmdBuffer);

        private:
            vk::SwapchainKHR mSwapchain;
            vk::SurfaceFormatKHR mSurfaceFormat;
            vk::PresentModeKHR mPresentMode = vk::PresentModeKHR::eFifo;
            vk::Extent2D mExtent;
            size_t mCurrFrame = 0;
            size_t mLastFrame = 0;

            uint32_t mImageCount = 2;
            std::vector<vk::Image> mImages;
            std::vector<vk::ImageView> mImageViews;

            std::vector<Semaphore> mImageAvlSph;
            std::vector<Semaphore> mRenderFinishedSph;
            std::vector<Fence> mInFlightFences;

            SwapchainSupportDetails mSupportDetails;

            vk::Extent2D chooseExtent(const vk::Extent2D& _rqExtent);

            bool choosePresentMode(const std::vector<vk::PresentModeKHR>& _rqPresentModes,
                                   vk::PresentModeKHR& _presentMode);

            bool chooseFormat(const std::vector<vk::SurfaceFormatKHR>& _rqFormats, VkSurfaceFormatKHR& _format);

            void createSwapchainImageView();
        };
    }
}
#endif // !MX_VK_SWAPCHAIN_H_

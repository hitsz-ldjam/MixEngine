#pragma once
#ifndef MX_VK_SWAPCHAIN_H_
#define MX_VK_SWAPCHAIN_H_

#include "../Device/MxVkDevice.h"
#include "../Image/MxVkImage.h"
#include "../SyncObject/MxVkSyncObject.h"

namespace Mix {
	namespace Graphics {
		class Swapchain : public GeneralBase::NoCopyBase {
		public:
			explicit Swapchain(const std::shared_ptr<Device>& _device);

			~Swapchain();

			void setImageCount(const uint32_t _count) {
				// We need at least two image to swap
				if (_count < 2)
					return;

				mImageCount = mSupportDetails.capabilities.minImageCount < _count ? _count : mSupportDetails.capabilities.minImageCount;
				mImageCount = mImageCount < mSupportDetails.capabilities.maxImageCount ? mImageCount : mSupportDetails.capabilities.maxImageCount;

			}

			Swapchain(Swapchain&& _other) noexcept { swap(_other); }

			Swapchain& operator=(Swapchain&& _other) noexcept { swap(_other); return *this; }

			void swap(Swapchain& _rhs) noexcept;

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

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			const vk::SurfaceKHR& getSurface() const { return mSurface; }

			const std::vector<vk::Image>& getImages() const { return mImages; }

			const std::vector<vk::ImageView>& getImageViews() const { return mImageViews; }

			const vk::Image& getCurrImage() const { return mImages[mCurrFrame]; }

			const vk::ImageView& getCurrImageView() const { return mImageViews[mCurrFrame]; }

			const vk::SurfaceFormatKHR& surfaceFormat() const { return mSurfaceFormat; }

			vk::PresentModeKHR presentMode() const { return mPresentMode; }

			const vk::Extent2D& extent() const { return mExtent; }

			uint32_t width() const { return mExtent.width; }

			uint32_t height() const { return mExtent.height; }

			uint32_t imageCount() const { return mImageCount; }

			size_t currFrame() const { return mCurrFrame; }

			vk::Result acquireNextImage();

			/**
			 * \brief Get the semaphore used to check whether "next" image is writable
			 */
			const vk::Semaphore& presentFinishedSph() const { return mImageAvlSph[mCurrFrame].get(); }

			/**
			 * \brief Get the semaphore used to notify the swap chain that
			 * every write operations on "this" image are finished and that
			 * swap chain can use "this" image to present
			 */
			const vk::Semaphore& renderFinishedSph() const { return mRenderFinishedSph[mCurrFrame].get(); }

			vk::Result present();

		private:
			std::shared_ptr<Device> mDevice;
			vk::SwapchainKHR mSwapchain;

			vk::SurfaceKHR mSurface;
			vk::SurfaceFormatKHR mSurfaceFormat;
			vk::PresentModeKHR mPresentMode = vk::PresentModeKHR::eFifo;
			vk::Extent2D mExtent;
			size_t mCurrFrame = 0;
			size_t mNextImage = 0;

			uint32_t mImageCount = 2;
			std::vector<vk::Image> mImages;
			std::vector<vk::ImageView> mImageViews;

			std::vector<Semaphore> mImageAvlSph;
			std::vector<Semaphore> mRenderFinishedSph;

			SwapchainSupportDetails mSupportDetails;

			vk::Extent2D chooseExtent(const vk::Extent2D& _rqExtent) const;

			bool choosePresentMode(const std::vector<vk::PresentModeKHR>& _rqPresentModes,
								   vk::PresentModeKHR& _presentMode);

			bool chooseFormat(const std::vector<vk::SurfaceFormatKHR>& _rqFormats, VkSurfaceFormatKHR& _format);

			void createSwapchainImageView();
		};
	}
}
#endif // !MX_VK_SWAPCHAIN_H_

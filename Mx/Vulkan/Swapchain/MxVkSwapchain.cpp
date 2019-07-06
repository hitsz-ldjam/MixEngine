#include "MxVkSwapchain.h"
#include "../Core/MxVkExcept.hpp"
#include "../../Log/MxLog.h"

namespace Mix {
	namespace Graphics {
		Swapchain::Swapchain(const std::shared_ptr<Device>& _device)
			:mDevice(_device), mSurface(_device->getSurface().value()), mSurfaceFormat() {
			auto physicalDevice = mDevice->getPhysicalDevice();
			mSupportDetails.capabilities =
				physicalDevice->get().getSurfaceCapabilitiesKHR(mSurface);

			mSupportDetails.formats =
				physicalDevice->get().getSurfaceFormatsKHR(mSurface);

			mSupportDetails.presentModes =
				physicalDevice->get().getSurfacePresentModesKHR(mSurface);
		}

		Swapchain::~Swapchain() {
			if (!mSwapchain)
				return;

			mDevice->get().destroySwapchainKHR(mSwapchain, nullptr, mDevice->getDynamicLoader());
			for (auto& view : mImageViews)
				mDevice->get().destroyImageView(view);
		}


		void Swapchain::swap(Swapchain& _rhs) noexcept {
			using std::swap;
			swap(mDevice, _rhs.mDevice);
			swap(mSwapchain, _rhs.mSwapchain);
			swap(mSurface, _rhs.mSurface);
			swap(mSurfaceFormat, _rhs.mSurfaceFormat);
			swap(mPresentMode, _rhs.mPresentMode);
			swap(mExtent, _rhs.mExtent);
			swap(mCurrFrame, _rhs.mCurrFrame);
			swap(mNextImage, _rhs.mNextImage);
			swap(mImageCount, _rhs.mImageCount);
			swap(mImages, _rhs.mImages);
			swap(mImageViews, _rhs.mImageViews);
			swap(mImageAvlSph, _rhs.mImageAvlSph);
			swap(mRenderFinishedSph, _rhs.mRenderFinishedSph);
			swap(mSupportDetails, _rhs.mSupportDetails);
		}

		void  Swapchain::create(const std::vector<vk::SurfaceFormatKHR>& _rqFormats,
								const std::vector<vk::PresentModeKHR>& _rqPresentMode,
								const vk::Extent2D& _rqExtent) {
			mImageAvlSph.resize(mImageCount);
			mRenderFinishedSph.resize(mImageCount);

			for (uint32_t i = 0; i < mImageCount; ++i) {
				mImageAvlSph[i] = Semaphore(mDevice);
				mRenderFinishedSph[i] = Semaphore(mDevice);
			}

			vk::SurfaceFormatKHR format;
			if (!chooseFormat(_rqFormats, format))
				throw SurfaceFormatUnsupported();

			vk::PresentModeKHR presentMode;
			if (!choosePresentMode(_rqPresentMode, presentMode))
				throw PresentModeUnsupported();

			VkExtent2D extent = chooseExtent(_rqExtent);

			vk::SwapchainCreateInfoKHR createInfo;
			createInfo.surface = mSurface;
			createInfo.presentMode = presentMode;
			createInfo.minImageCount = mImageCount;
			createInfo.imageFormat = format.format;
			createInfo.imageColorSpace = format.colorSpace;
			createInfo.imageExtent = extent;
			createInfo.imageArrayLayers = 1;
			createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

			uint32_t queueFamilyIndices[2] = {
				mDevice->getQueueFamilyIndexSet().graphics.value(),
				mDevice->getQueueFamilyIndexSet().present.value()
			};

			if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
				// if graphics queue and present queue belong to different queue family
				createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
				createInfo.pQueueFamilyIndices = queueFamilyIndices;
				createInfo.queueFamilyIndexCount = 2;
			}
			else { // graphics queue and present queue belong to same queue family
				createInfo.imageSharingMode = vk::SharingMode::eExclusive;
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}

			createInfo.preTransform = mSupportDetails.capabilities.currentTransform;
			createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
			createInfo.clipped = true;
			createInfo.oldSwapchain = nullptr;

			// create swapchain
			mSwapchain = mDevice->get().createSwapchainKHR(createInfo, nullptr, mDevice->getDynamicLoader());
			//acquire image in swapchain
			mImages = mDevice->get().getSwapchainImagesKHR(mSwapchain, mDevice->getDynamicLoader());
			//stroe
			mSurfaceFormat = format;
			mPresentMode = presentMode;
			mExtent = extent;
			createSwapchainImageView();
		}

		vk::Extent2D  Swapchain::chooseExtent(const vk::Extent2D & _rqExtent) const {
			if (mSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return mSupportDetails.capabilities.currentExtent;
			}
			else {
				VkExtent2D actualExtent = { static_cast<uint32_t>(_rqExtent.width), static_cast<uint32_t>(_rqExtent.height) };
				actualExtent.width = std::max(mSupportDetails.capabilities.minImageExtent.width,
											  std::min(mSupportDetails.capabilities.maxImageExtent.width, actualExtent.width));
				actualExtent.height = std::max(mSupportDetails.capabilities.minImageExtent.height,
											   std::min(mSupportDetails.capabilities.maxImageExtent.height, actualExtent.height));
				return actualExtent;
			}
		}

		bool  Swapchain::choosePresentMode(const std::vector<vk::PresentModeKHR>& _rqPresentModes, vk::PresentModeKHR & _presentMode) {
			for (auto rqPresentMode : _rqPresentModes) {
				for (auto support : mSupportDetails.presentModes)
					if (rqPresentMode == support) {
						_presentMode = rqPresentMode;
						return true;
					}
			}
			return false;
		}

		bool  Swapchain::chooseFormat(const std::vector<vk::SurfaceFormatKHR>& _rqFormats, VkSurfaceFormatKHR & _format) {
			if (mSupportDetails.formats.size() == 1 &&
				mSupportDetails.formats[0].format == vk::Format::eUndefined) {
				_format = _rqFormats[0];
				return true;
			}

			for (const auto& rqFormat : _rqFormats) {
				for (const auto& support : mSupportDetails.formats) {
					if (rqFormat.format == support.format && rqFormat.colorSpace == support.colorSpace) {
						_format = support;
						return true;
					}
				}
			}
			return false;
		}

		void  Swapchain::createSwapchainImageView() {
			mImageViews.resize(mImages.size());
			for (size_t i = 0; i < mImages.size(); ++i)
				mImageViews[i] = Image::CreateVkImageView2D(mDevice->get(),
															mImages[i],
															mSurfaceFormat.format,
															vk::ImageAspectFlagBits::eColor,
															0, 1,
															0, 1);
		}

		vk::Result Swapchain::acquireNextImage() {
			// Acquire next image
			// The presentation engine may not have finished reading 
			// from the image at the time it is acquired
			// We use mImageAvlSph[mCurrFrame] semaphore check later whether next image is ready
			auto acquireResult = mDevice->get().acquireNextImageKHR(mSwapchain,
																	std::numeric_limits<uint64_t>::max(),
																	mImageAvlSph[mCurrFrame].get(),
																	nullptr);

			if (acquireResult.result != vk::Result::eSuccess &&
				acquireResult.result != vk::Result::eSuboptimalKHR &&
				acquireResult.result != vk::Result::eErrorOutOfDateKHR) {
				throw Exception("Failed to acquire next image");
			}

			mNextImage = acquireResult.value;
			return acquireResult.result;
		}

		vk::Result Swapchain::present() {
			//swapchain present
			vk::PresentInfoKHR presentInfo = {};
			presentInfo.pWaitSemaphores = &mRenderFinishedSph[mCurrFrame].get();
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pSwapchains = &mSwapchain;
			presentInfo.swapchainCount = 1;
			presentInfo.pImageIndices = &mNextImage;
			presentInfo.pResults = nullptr;

			const auto result = mDevice->getQueueSet().present.value().presentKHR(presentInfo);

			if (result != vk::Result::eSuccess &&
				result != vk::Result::eSuboptimalKHR &&
				result != vk::Result::eErrorOutOfDateKHR)
				throw Exception("Failed to present image");

			mCurrFrame = (mCurrFrame + 1) % mImageCount;
			return result;
		}
	}
}

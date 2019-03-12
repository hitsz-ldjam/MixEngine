#include "MxVkSwapchain.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Swapchain, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Swapchain);

        void  Swapchain::init(const Core * core) {
            mCore = core;
            mSupportDetails.capabilities =
                mCore->physicalDevice().getSurfaceCapabilitiesKHR(mCore->surface());

            mSupportDetails.formats =
                mCore->physicalDevice().getSurfaceFormatsKHR(mCore->surface());

            mSupportDetails.presentModes =
                mCore->physicalDevice().getSurfacePresentModesKHR(mCore->surface());

            mImageCount = mSupportDetails.capabilities.minImageCount;
            if (mImageCount > 0 && mImageCount > mSupportDetails.capabilities.maxImageCount)
                mImageCount = mSupportDetails.capabilities.maxImageCount;
        }

        void Swapchain::initSyncObj(SyncObjectMgr & syncMgr) {
            mImageAvlSph.resize(mImageCount);
            mRenderFinishedSph.resize(mImageCount);
            mInFlightFences.resize(mImageCount);

            for (uint32_t i = 0; i < mImageCount; ++i) {
                mImageAvlSph[i] = syncMgr.createSemaphore();
                mRenderFinishedSph[i] = syncMgr.createSemaphore();
                mInFlightFences[i] = syncMgr.createFence();
            }
        }

        void Swapchain::destroy() {
            for (auto& view : mImageViews)
                mCore->device().destroyImageView(view);
            mCore->device().destroySwapchainKHR(mSwapchain);
        }

        void  Swapchain::create(const std::vector<vk::SurfaceFormatKHR> & rqFormats, const vk::PresentModeKHR rqPresentMode, const vk::Extent2D & rqExtent) {
            vk::SurfaceFormatKHR format;
            if (!chooseFormat(rqFormats, format))
                throw SurfaceFormatUnsupported();

            vk::PresentModeKHR presentMode;
            if (!choosePresentMode(rqPresentMode, presentMode))
                throw PresentModeUnsupported();

            VkExtent2D extent = chooseExtent(rqExtent);

            vk::SwapchainCreateInfoKHR createInfo = {};
            createInfo.surface = mCore->surface();
            createInfo.presentMode = presentMode;
            createInfo.minImageCount = mImageCount;
            createInfo.imageFormat = format.format;
            createInfo.imageColorSpace = format.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment; //imageµÄÓÃÍ¾

            uint32_t queueFamilyIndices[2] = {
                mCore->getQueueFamilyIndices().graphics.value(),
                mCore->getQueueFamilyIndices().present.value()
            };

            if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
                // if graphics queue and present queue belong to different queue family
                createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
                createInfo.queueFamilyIndexCount = 2;
            } else { // graphics queue and present queue belong to same queue family
                createInfo.imageSharingMode = vk::SharingMode::eExclusive;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices = nullptr;
            }

            createInfo.preTransform = mSupportDetails.capabilities.currentTransform;
            createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            createInfo.clipped = true;
            createInfo.oldSwapchain = nullptr;

            // create swapchain
            mSwapchain = mCore->device().createSwapchainKHR(createInfo, nullptr, mCore->dynamicLoader());
            //acquire image in swapchain
            mImages = mCore->device().getSwapchainImagesKHR(mSwapchain, mCore->dynamicLoader());
            //stroe
            mSurfaceFormat = format;
            mPresentMode = presentMode;
            mExtent = extent;
            createSwapchainImageView();
        }

        vk::Extent2D  Swapchain::chooseExtent(const vk::Extent2D & rqExtent) {
            if (mSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return mSupportDetails.capabilities.currentExtent;
            } else {
                VkExtent2D actualExtent = { static_cast<uint32_t>(rqExtent.width), static_cast<uint32_t>(rqExtent.height) };
                actualExtent.width = std::max(mSupportDetails.capabilities.minImageExtent.width,
                                              std::min(mSupportDetails.capabilities.maxImageExtent.width, actualExtent.width));
                actualExtent.height = std::max(mSupportDetails.capabilities.minImageExtent.height,
                                               std::min(mSupportDetails.capabilities.maxImageExtent.height, actualExtent.height));
                return actualExtent;
            }
        }

        bool  Swapchain::choosePresentMode(const vk::PresentModeKHR rqPresentMode, vk::PresentModeKHR & presentMode) {
            for (const auto& support : mSupportDetails.presentModes) {
                if (rqPresentMode == support) {
                    presentMode = support;
                    return true;
                }
            }
            return false;
        }

        bool  Swapchain::chooseFormat(const std::vector<vk::SurfaceFormatKHR>& rqFormats, VkSurfaceFormatKHR & format) {
            if (mSupportDetails.formats.size() == 1 &&
                mSupportDetails.formats[0].format == vk::Format::eUndefined) {
                format = rqFormats[0];
                return true;
            }

            for (const auto& rqFormat : rqFormats) {
                for (const auto& support : mSupportDetails.formats) {
                    if (rqFormat.format == support.format && rqFormat.colorSpace == support.colorSpace) {
                        format = support;
                        return true;
                    }
                }
            }
            return false;
        }

        void  Swapchain::createSwapchainImageView() {
            mImageViews.resize(mImages.size());
            for (size_t i = 0; i < mImages.size(); ++i)
                mImageViews[i] = Tools::createImageView2D(mCore->device(),
                                                          mImages[i],
                                                          mSurfaceFormat.format,
                                                          vk::ImageAspectFlagBits::eColor,
                                                          0, 1,
                                                          0, 1);
        }

        void Swapchain::present(vk::CommandBuffer& cmdBuffer) {
            mCore->device().waitForFences(mInFlightFences[mCurrFrame].get(),
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());

            mCore->device().resetFences(mInFlightFences[mCurrFrame].get());

            auto acquireResult = mCore->device().acquireNextImageKHR(mSwapchain,
                                                                     std::numeric_limits<uint64_t>::max(),
                                                                     mImageAvlSph[mCurrFrame].get(),
                                                                     nullptr);

            if (acquireResult.result == vk::Result::eErrorOutOfDateKHR ||
                acquireResult.result == vk::Result::eSuboptimalKHR) {
                std::cerr << "Error : Swapchain out of date" << std::endl;
                return;
            } else if (acquireResult.result != vk::Result::eSuccess)
                throw SwapchainSwapFailed();

            //submit command buffer
            vk::Semaphore waitSemaphores[] = { mImageAvlSph[mCurrFrame].get() };
            vk::Semaphore signalSemaphores[] = { mRenderFinishedSph[mCurrFrame].get() };
            vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

            vk::SubmitInfo submitInfo = {};
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.pSignalSemaphores = signalSemaphores;
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pCommandBuffers = &cmdBuffer;
            submitInfo.commandBufferCount = 1;

            mCore->getQueues().graphics.value().submit(submitInfo, mInFlightFences[mCurrFrame].get());

            //swapchain present
            vk::PresentInfoKHR presentInfo = {};
            presentInfo.pWaitSemaphores = signalSemaphores;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pSwapchains = &mSwapchain;
            presentInfo.swapchainCount = 1;
            presentInfo.pImageIndices = &acquireResult.value;
            presentInfo.pResults = nullptr;

            auto result = mCore->getQueues().present.value().presentKHR(presentInfo);

            if (result == vk::Result::eErrorOutOfDateKHR ||
                result == vk::Result::eSuboptimalKHR) {
                std::cerr << "Error : Swapchain out of date" << std::endl;
                return;
            } else if (acquireResult.result != vk::Result::eSuccess)
                throw SwapchainSwapFailed();

            mCurrFrame = (mCurrFrame + 1) % mImageCount;
        }
    }
}
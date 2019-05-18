#include "MxVkSwapchain.h"

namespace Mix {
    namespace Graphics {
        void Swapchain::init(const std::shared_ptr<Core>& _core){
            setCore(_core);
            mSupportDetails.capabilities =
                mCore->getPhysicalDevice().getSurfaceCapabilitiesKHR(mCore->getSurface());

            mSupportDetails.formats =
                mCore->getPhysicalDevice().getSurfaceFormatsKHR(mCore->getSurface());

            mSupportDetails.presentModes =
                mCore->getPhysicalDevice().getSurfacePresentModesKHR(mCore->getSurface());
        }

        void Swapchain::destroy() {
            if (!mCore)
                return;

            for (auto& view : mImageViews)
                mCore->getDevice().destroyImageView(view);
            mCore->getDevice().destroySwapchainKHR(mSwapchain);
            mCore = nullptr;
        }

        void  Swapchain::create(const std::vector<vk::SurfaceFormatKHR>& _rqFormats, 
                                const std::vector<vk::PresentModeKHR>& _rqPresentMode, 
                                const vk::Extent2D& _rqExtent) {
            mImageAvlSph.resize(mImageCount);
            mRenderFinishedSph.resize(mImageCount);
            mInFlightFences.resize(mImageCount);

            for (uint32_t i = 0; i < mImageCount; ++i) {
                mImageAvlSph[i] = mCore->getSyncObjMgr().createSemaphore();
                mRenderFinishedSph[i] = mCore->getSyncObjMgr().createSemaphore();
                mInFlightFences[i] = mCore->getSyncObjMgr().createFence();
            }

            // to keep thins in present() right
            for (uint32_t i = 1; i < mImageCount; ++i) {
                mCore->getDevice().resetFences(mInFlightFences[i].get());
            }

            vk::SurfaceFormatKHR format;
            if (!chooseFormat(_rqFormats, format))
                throw SurfaceFormatUnsupported();

            vk::PresentModeKHR presentMode;
            if (!choosePresentMode(_rqPresentMode, presentMode))
                throw PresentModeUnsupported();

            VkExtent2D extent = chooseExtent(_rqExtent);

            vk::SwapchainCreateInfoKHR createInfo = {};
            createInfo.surface = mCore->getSurface();
            createInfo.presentMode = presentMode;
            createInfo.minImageCount = mImageCount;
            createInfo.imageFormat = format.format;
            createInfo.imageColorSpace = format.colorSpace;
            createInfo.imageExtent = extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

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
            mSwapchain = mCore->getDevice().createSwapchainKHR(createInfo, nullptr, mCore->dynamicLoader());
            //acquire image in swapchain
            mImages = mCore->getDevice().getSwapchainImagesKHR(mSwapchain, mCore->dynamicLoader());
            //stroe
            mSurfaceFormat = format;
            mPresentMode = presentMode;
            mExtent = extent;
            createSwapchainImageView();
        }

        vk::Extent2D  Swapchain::chooseExtent(const vk::Extent2D & _rqExtent) {
            if (mSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return mSupportDetails.capabilities.currentExtent;
            } else {
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
                mImageViews[i] = Tools::CreateImageView2D(mCore->getDevice(),
                                                          mImages[i],
                                                          mSurfaceFormat.format,
                                                          vk::ImageAspectFlagBits::eColor,
                                                          0, 1,
                                                          0, 1);
        }

        void Swapchain::present(vk::CommandBuffer& _cmdBuffer) {
            mCore->getDevice().waitForFences(mInFlightFences[mLastFrame].get(),
                                          VK_TRUE,
                                          std::numeric_limits<uint64_t>::max());

            mCore->getDevice().resetFences(mInFlightFences[mLastFrame].get());

            auto acquireResult = mCore->getDevice().acquireNextImageKHR(mSwapchain,
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
            submitInfo.pCommandBuffers = &_cmdBuffer;
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

            mLastFrame = mCurrFrame;
            mCurrFrame = (mCurrFrame + 1) % mImageCount;
        }
    }
}
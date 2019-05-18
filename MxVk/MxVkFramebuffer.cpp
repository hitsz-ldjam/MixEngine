#include "MxVkFramebuffer.h"

namespace Mix {
    namespace Graphics {
        void Framebuffer::clear() {
            mAttachments.clear();
        }

        void Framebuffer::addAttachments(const std::vector<vk::ImageView>& _attachments) {
            mAttachments.insert(mAttachments.end(), _attachments.cbegin(), _attachments.cend());
        }

        void Framebuffer::create() {
            vk::FramebufferCreateInfo createInfo;
            createInfo.renderPass = mRenderPass;
            createInfo.pAttachments = mAttachments.data();
            createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
            createInfo.width = mExtent.width;
            createInfo.height = mExtent.height;
            createInfo.layers = mLayers;

            mFramebuffer = mCore->getDevice().createFramebuffer(createInfo);
        }

        void Framebuffer::destroy() {
            if (!mCore)
                return;

            mCore->getDevice().destroyFramebuffer(mFramebuffer);
            mCore = nullptr;
            mAttachments.clear();
            mRenderPass = nullptr;
            mFramebuffer = nullptr;
            mExtent = vk::Extent2D();
            mLayers = 0;
        }
    }
}
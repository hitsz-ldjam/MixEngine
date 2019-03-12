#include "MxVkFramebuffer.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Framebuffer, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Framebuffer);

        void Framebuffer::clear() {
            mAttachments.clear();
        }

        void Framebuffer::addAttachments(const std::vector<vk::ImageView>& attachments) {
            mAttachments.insert(mAttachments.end(), attachments.cbegin(), attachments.cend());
        }

        void Framebuffer::create() {
            vk::FramebufferCreateInfo createInfo;
            createInfo.renderPass = mRenderPass;
            createInfo.pAttachments = mAttachments.data();
            createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
            createInfo.width = mExtent.width;
            createInfo.height = mExtent.height;
            createInfo.layers = mLayers;

            mFramebuffer = mCore->device().createFramebuffer(createInfo);
        }

        void Framebuffer::destroy() {
            if (!mCore)
                return;

            mCore->device().destroyFramebuffer(mFramebuffer);
            mCore = nullptr;
            mAttachments.clear();
            mRenderPass = nullptr;
            mFramebuffer = nullptr;
            mExtent = {  };
            mLayers = 0;
        }
    }
}
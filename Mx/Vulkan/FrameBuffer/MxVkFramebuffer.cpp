#include "MxVkFramebuffer.h"

namespace Mix {
    namespace Graphics {
        void FrameBuffer::clear() {
            mAttachments.clear();
        }

        void FrameBuffer::swap(FrameBuffer& _other) noexcept {
	        using std::swap;
	        swap(mFramebuffer, _other.mFramebuffer);
	        swap(mRenderPass, _other.mRenderPass);
	        swap(mExtent, _other.mExtent);
	        swap(mLayers, _other.mLayers);
	        swap(mAttachments, _other.mAttachments);
        }

        FrameBuffer::~FrameBuffer() {
			if (!mRenderPass)
				return;

			mRenderPass->getDevice()->get().destroyFramebuffer(mFramebuffer);
        }

        void FrameBuffer::addAttachments(const std::vector<vk::ImageView>& _attachments) {
            mAttachments.insert(mAttachments.end(), _attachments.cbegin(), _attachments.cend());
        }

        void FrameBuffer::create() {
            vk::FramebufferCreateInfo createInfo;
            createInfo.renderPass = mRenderPass->get();
            createInfo.pAttachments = mAttachments.data();
            createInfo.attachmentCount = static_cast<uint32_t>(mAttachments.size());
            createInfo.width = mExtent.width;
            createInfo.height = mExtent.height;
            createInfo.layers = mLayers;

            mFramebuffer = mRenderPass->getDevice()->get().createFramebuffer(createInfo);
        }
    }
}
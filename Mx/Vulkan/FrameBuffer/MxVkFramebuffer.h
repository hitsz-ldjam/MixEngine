#pragma once
#ifndef MX_VK_FRAMEBUFFER_H_
#define MX_VK_FRAMEBUFFER_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "../Pipeline/MxVkRenderPass.h"

namespace Mix {
	namespace Graphics {
		class Device;

		class FrameBuffer : public GeneralBase::NoCopyBase {
		public:
			FrameBuffer(const std::shared_ptr<RenderPass>& _renderPass,
						const vk::Extent2D& _extent,
						const uint32_t _layer = 1)
				: mRenderPass(_renderPass), mExtent(_extent), mLayers(_layer) {
			}


			FrameBuffer(FrameBuffer&& _other) noexcept { swap(_other); }

			FrameBuffer& operator=(FrameBuffer&& _other) noexcept { swap(_other); return *this; }

			void swap(FrameBuffer& _other) noexcept;

			~FrameBuffer();

			void addAttachments(const std::vector<vk::ImageView>& _attachments);

			void create();

			const vk::Framebuffer& get() const { return mFramebuffer; }

			const std::vector<vk::ImageView>& getAttachments() const { return mAttachments; }

			std::shared_ptr<RenderPass> getRenderPass() const { return mRenderPass; }

		private:
			vk::Framebuffer mFramebuffer;
			std::shared_ptr<RenderPass> mRenderPass;
			vk::Extent2D mExtent;
			uint32_t mLayers = 0;

			std::vector<vk::ImageView> mAttachments;
			void clear();
		};
	}
}
#endif // !MX_VK_FRAMEBUFFER_H_

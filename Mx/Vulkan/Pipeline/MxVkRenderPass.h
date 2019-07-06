#pragma once
#ifndef MX_VK_RENDERPASS_H_
#define MX_VK_RENDERPASS_H_

#include "../Device/MxVkDevice.h"
#include <set>

namespace Mix {
	namespace Graphics {
		class Attachment {
		public:
			enum class Type {
				IMAGE, DEPTH_STENCIL, PRESENT
			};

			Attachment(const uint32_t _binding,
					   const Type _type,
					   const vk::Format _format = vk::Format::eR8G8B8A8Unorm,
					   const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
					   const vk::AttachmentLoadOp _loadOp = vk::AttachmentLoadOp::eDontCare,
					   const vk::AttachmentStoreOp _storeOp = vk::AttachmentStoreOp::eDontCare,
					   const vk::AttachmentLoadOp _stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
					   const vk::AttachmentStoreOp _stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
					   const vk::ImageLayout _initLayout = vk::ImageLayout::eUndefined,
					   const vk::ImageLayout _finalLayout = vk::ImageLayout::eUndefined,
					   const vk::AttachmentDescriptionFlags _flags = {});

			bool operator<(const Attachment& _other) const {
				return mBinding < _other.mBinding;
			}

			uint32_t binding() const { return mBinding; }

			Type type() const { return mType; }

			const vk::AttachmentDescription& get() const { return mDescription; }

		private:
			uint32_t mBinding;
			Type mType;
			vk::AttachmentDescription mDescription;
		};

		class AttachmentRef {
		public:
			enum class Type {
				INPUT, COLOR, RESOLVE, DEPTH_STENCIL
			};

			AttachmentRef(const Type _type,
						  const uint32_t _attach,
						  const vk::ImageLayout _layout = vk::ImageLayout::eUndefined);

			Type type() const { return mType; }

			const vk::AttachmentReference& get() const { return mRef; }

		private:
			Type mType;
			vk::AttachmentReference mRef;
		};

		class Subpass {
		public:
			explicit Subpass(const uint32_t _index,
							 std::initializer_list<AttachmentRef> _refs = {},
							 std::initializer_list<uint32_t> _preserves = {},
							 const vk::SubpassDescriptionFlags _flags = {});

			bool operator<(const Subpass& _other) const { return mIndex < _other.mIndex; }

			vk::SubpassDescription get() const;

			void addRef(std::initializer_list<AttachmentRef> _refs);

			void addRef(const AttachmentRef& _ref);

			void addPreserve(std::initializer_list<uint32_t> _preserves);

			void addPreserve(const uint32_t _preserve);

		private:
			uint32_t mIndex;
			vk::SubpassDescriptionFlags mFlags;
			std::vector<vk::AttachmentReference> mInput;
			std::vector<vk::AttachmentReference> mColor;
			std::optional<vk::AttachmentReference> mResolve;
			std::optional<vk::AttachmentReference> mDepthStencil;
			std::vector<uint32_t> mPreserve;
		};

		class RenderPass : public GeneralBase::NoCopyBase {
		public:
			explicit RenderPass(const std::shared_ptr<Device>& _device);

			~RenderPass() { destroy(); }

			RenderPass(RenderPass&& _other) noexcept { swap(_other); }

			RenderPass& operator=(RenderPass&& _other) noexcept { swap(_other); return *this; }

			void swap(RenderPass& _other) noexcept;

			void create();

			const vk::RenderPass& get() const { return mRenderPass; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			void beginRenderPass(const vk::CommandBuffer& _cmdBuffer,
								 const vk::Framebuffer& _frameBuffer,
								 std::vector<vk::ClearValue>& _clearValues,
								 const vk::Extent2D& _extent,
								 const vk::Offset2D& _offset = { 0,0 },
								 const vk::SubpassContents _subpassContent = vk::SubpassContents::eInline);

			void endRenderPass(const vk::CommandBuffer& _cmdBuffer);

			void destroy();

			void addAttachment(std::initializer_list<Attachment> _attachments);

			void addAttachment(const Attachment& _attachment);

			void addSubpass(std::initializer_list<Subpass> _subpasses);

			void addSubpass(const Subpass& _subpass);

			void addDependency(const vk::SubpassDependency& _dependency);

			void addDependency(std::initializer_list<vk::SubpassDependency> _dependencies);

		private:
			std::shared_ptr<Device> mDevice;
			vk::RenderPass mRenderPass;

			std::shared_ptr<std::set<Attachment>> mAttachments;
			std::shared_ptr<std::set<Subpass>> mSubpasses;
			std::shared_ptr<std::vector<vk::SubpassDependency>> mDependencies;
		};
	}
}
#endif // !MX_VK_RENDERPASS_H_

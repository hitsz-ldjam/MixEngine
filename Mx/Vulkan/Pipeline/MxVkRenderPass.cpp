#include "MxVkRenderPass.h"

namespace Mix {
	namespace Vulkan {
		Attachment::Attachment(const uint32_t _binding,
							   const Type _type,
							   const vk::Format _format,
							   const vk::SampleCountFlagBits _sampleCount,
							   const vk::AttachmentLoadOp _loadOp,
							   const vk::AttachmentStoreOp _storeOp,
							   const vk::AttachmentLoadOp _stencilLoadOp,
							   const vk::AttachmentStoreOp _stencilStoreOp,
							   const vk::ImageLayout _initLayout,
							   const vk::ImageLayout _finalLayout,
							   const vk::AttachmentDescriptionFlags _flags)
			: mBinding(_binding),
			mType(_type) {
			mDescription.flags = _flags;
			mDescription.format = _format;
			mDescription.samples = _sampleCount;

			switch (mType) {
			case Type::IMAGE:
				mDescription.loadOp = _loadOp;
				mDescription.storeOp = _storeOp;
				mDescription.stencilLoadOp = _stencilLoadOp;
				mDescription.stencilStoreOp = _stencilStoreOp;
				mDescription.initialLayout = _initLayout;
				mDescription.finalLayout = _finalLayout;
				break;
			case Type::DEPTH_STENCIL:
				mDescription.loadOp = _loadOp == vk::AttachmentLoadOp::eDontCare ? vk::AttachmentLoadOp::eClear : _loadOp;
				mDescription.storeOp = _storeOp;
				mDescription.stencilLoadOp = _stencilLoadOp;
				mDescription.stencilStoreOp = _stencilStoreOp;
				mDescription.initialLayout = _initLayout;
				mDescription.finalLayout = _finalLayout == vk::ImageLayout::eUndefined ? vk::ImageLayout::eDepthStencilAttachmentOptimal : _finalLayout;
				break;
			case Type::PRESENT:
				mDescription.loadOp = _loadOp == vk::AttachmentLoadOp::eDontCare ? vk::AttachmentLoadOp::eClear : _loadOp;
				mDescription.storeOp = _storeOp == vk::AttachmentStoreOp::eDontCare ? vk::AttachmentStoreOp::eStore : _storeOp;
				mDescription.stencilLoadOp = _stencilLoadOp;
				mDescription.stencilStoreOp = _stencilStoreOp;
				mDescription.initialLayout = _initLayout;
				mDescription.finalLayout = _finalLayout == vk::ImageLayout::eUndefined ? vk::ImageLayout::ePresentSrcKHR : _finalLayout;
				break;
			}
		}

		AttachmentRef::AttachmentRef(const Type _type,
									 const uint32_t _attach,
									 const vk::ImageLayout _layout)
			:mType(_type), mRef(_attach) {
			if(_layout==vk::ImageLayout::eUndefined) {
				switch (_type) {
				case Type::INPUT: mRef.layout = vk::ImageLayout::eShaderReadOnlyOptimal; break;
				case Type::COLOR: mRef.layout = vk::ImageLayout::eColorAttachmentOptimal; break;
				case Type::RESOLVE: mRef.layout = vk::ImageLayout::eColorAttachmentOptimal; break;
				case Type::DEPTH_STENCIL:  mRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal; break;
				default: ;
				}
			}
		}

		Subpass::Subpass(const uint32_t _index,
						 std::initializer_list<AttachmentRef> _refs,
						 std::initializer_list<uint32_t> _preserves,
						 const vk::SubpassDescriptionFlags _flags)
			: mIndex(_index), mFlags(_flags) {
			addRef(_refs);
			addPreserve(_preserves);
		}

		vk::SubpassDescription Subpass::get() const {
			return vk::SubpassDescription(mFlags,
										  vk::PipelineBindPoint::eGraphics,
										  static_cast<uint32_t>(mInput.size()),
										  mInput.data(),
										  static_cast<uint32_t>(mColor.size()),
										  mColor.data(),
										  mResolve ? &mResolve.value() : nullptr,
										  mDepthStencil ? &mDepthStencil.value() : nullptr,
										  static_cast<uint32_t>(mPreserve.size()),
										  mPreserve.data());
		}

		void Subpass::addRef(std::initializer_list<AttachmentRef> _refs) {
			for (auto& ref : _refs) {
				switch (ref.type()) {
				case AttachmentRef::Type::INPUT: mInput.push_back(ref.get()); break;
				case AttachmentRef::Type::COLOR: mColor.push_back(ref.get()); break;
				case AttachmentRef::Type::RESOLVE: mResolve = ref.get(); break;
				case AttachmentRef::Type::DEPTH_STENCIL: mDepthStencil = ref.get(); break;
				}
			}
		}

		void Subpass::addRef(const AttachmentRef& _ref) {
			switch (_ref.type()) {
			case AttachmentRef::Type::INPUT: mInput.push_back(_ref.get()); break;
			case AttachmentRef::Type::COLOR: mColor.push_back(_ref.get()); break;
			case AttachmentRef::Type::RESOLVE: mResolve = _ref.get(); break;
			case AttachmentRef::Type::DEPTH_STENCIL: mDepthStencil = _ref.get(); break;
			}
		}

		void Subpass::addPreserve(std::initializer_list<uint32_t> _preserves) {
			mPreserve.insert(mPreserve.end(), _preserves);
		}

		void Subpass::addPreserve(const uint32_t _preserve) {
			mPreserve.emplace_back(_preserve);
		}

		RenderPass::RenderPass(const std::shared_ptr<Device>& _device)
			: mDevice(_device),
			mAttachments(new std::set<Attachment>),
			mDependencies(new std::vector<vk::SubpassDependency>()),
			mSubpasses(new std::set<Subpass>()) {

		}

		/*uint32_t RenderPass::addColorAttach(const vk::Format _format,
											const vk::SampleCountFlagBits _sampleCount,
											const vk::AttachmentLoadOp _loadOp,
											const vk::AttachmentStoreOp _storeOp,
											const vk::ImageLayout _initLayout,
											const vk::ImageLayout _finalLayout) {
			mAttachments->emplace_back(vk::AttachmentDescriptionFlags(),
									   _format,
									   _sampleCount,
									   _loadOp, _storeOp,
									   vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
									   _initLayout, _finalLayout);
			return mAttachments->size() - 1;
		}

		uint32_t RenderPass::addDepthStencilAttach(const vk::Format _format,
												   const vk::SampleCountFlagBits _sampleCount,
												   const vk::AttachmentLoadOp _loadOp,
												   const vk::AttachmentStoreOp _storeOp,
												   const vk::AttachmentLoadOp _stencilLoadOp,
												   const vk::AttachmentStoreOp _stencilStoreOp,
												   const vk::ImageLayout _initLayout,
												   const vk::ImageLayout _finalLayout) {
			mAttachments->emplace_back(vk::AttachmentDescriptionFlags(),
									   _format,
									   _sampleCount,
									   _loadOp, _storeOp,
									   _stencilLoadOp, _stencilStoreOp,
									   _initLayout, _finalLayout);
			return mAttachments->size() - 1;
		}*/

		/*uint32_t RenderPass::addColorAttachRef(const uint32_t _index, const vk::ImageLayout _layout) {
			mAttachRefs->emplace_back(_index, _layout);
			return mAttachRefs->size() - 1;
		}

		uint32_t RenderPass::addDepthStencilAttachRef(const uint32_t _index, const vk::ImageLayout _layout) {
			mAttachRefs->emplace_back(_index, _layout);
			return mAttachRefs->size() - 1;
		}

		uint32_t RenderPass::addSubpass(const vk::PipelineBindPoint _bindPoint) {
			SubpassContent subpass;
			subpass.bindPoint = _bindPoint;
			mSubpasses->push_back(std::move(subpass));
			return mSubpasses->size() - 1;
		}

		bool RenderPass::addSubpassColorRef(const uint32_t _subpassIndex,
											const std::vector<uint32_t>& _refIndices) {
			for (auto& index : _refIndices) {
				if (!(index < mAttachRefs->size()))
					return false;
			}
			auto& subpass = (*mSubpasses)[_subpassIndex];
			subpass.colorRef.reserve(subpass.colorRef.size() + _refIndices.size());
			for (const auto& index : _refIndices) {
				subpass.colorRef.push_back((*mAttachRefs)[index]);
			}
			return true;
		}*/

		/*bool RenderPass::addSubpassColorRef(const uint32_t _subpassIndex, const uint32_t _refIndex) {
			if (!(_refIndex < mAttachRefs->size()))
				return false;
			(*mSubpasses)[_subpassIndex].colorRef.push_back((*mAttachRefs)[_refIndex]);
			return true;
		}

		bool RenderPass::addSubpassDepthStencilRef(const uint32_t _subpassIndex, const uint32_t _refIndex) {
			if (!(_refIndex < mAttachRefs->size()))
				return false;
			(*mSubpasses)[_subpassIndex].depthStencilRef = (*mAttachRefs)[_refIndex];
			(*mSubpasses)[_subpassIndex].hasDepthStencilRef = true;
			return true;
		}

		bool RenderPass::addSubpassResolveRef(const uint32_t _subpassIndex, const uint32_t _refIndex) {
			if (!(_refIndex < mAttachRefs->size()))
				return false;
			(*mSubpasses)[_subpassIndex].resolveRef = (*mAttachRefs)[_refIndex];
			(*mSubpasses)[_subpassIndex].hasResolveRef = true;
			return true;
		}*/

		void RenderPass::swap(RenderPass& _other) noexcept {
			using std::swap;
			swap(mDevice, _other.mDevice);
			swap(mRenderPass, _other.mRenderPass);
			swap(mAttachments, _other.mAttachments);
			swap(mSubpasses, _other.mSubpasses);
			swap(mDependencies, _other.mDependencies);
		}

		void RenderPass::create() {
			std::vector<vk::AttachmentDescription> attachments;
			attachments.reserve(mAttachments->size());
			for (auto& attach : *mAttachments)
				attachments.emplace_back(attach.get());

			std::vector<vk::SubpassDescription> subpasses;
			subpasses.reserve(mSubpasses->size());
			for (auto& subpass : *mSubpasses)
				subpasses.emplace_back(subpass.get());

			vk::RenderPassCreateInfo createInfo;
			createInfo.pAttachments = attachments.data();
			createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			createInfo.pSubpasses = subpasses.data();
			createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
			createInfo.pDependencies = mDependencies->data();
			createInfo.dependencyCount = static_cast<uint32_t>(mDependencies->size());

			mRenderPass = mDevice->get().createRenderPass(createInfo);
		}

		void RenderPass::beginRenderPass(const vk::CommandBuffer& _cmdBuffer, const vk::Framebuffer& _frameBuffer,
										 std::vector<vk::ClearValue>& _clearValues, const vk::Extent2D& _extent,
										 const vk::Offset2D& _offset, const vk::SubpassContents _subpassContent) {
			vk::RenderPassBeginInfo beginInfo;
			beginInfo.renderPass = mRenderPass;
			beginInfo.framebuffer = _frameBuffer;
			beginInfo.pClearValues = _clearValues.data();
			beginInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
			beginInfo.renderArea.offset = _offset;
			beginInfo.renderArea.extent = _extent;

			_cmdBuffer.beginRenderPass(beginInfo, _subpassContent);
		}

		void RenderPass::endRenderPass(const vk::CommandBuffer& _cmdBuffer) {
			_cmdBuffer.endRenderPass();
		}

		void RenderPass::destroy() {
			if (!mDevice)
				return;

			mDevice->get().destroyRenderPass(mRenderPass);
			mDevice = nullptr;
		}

		void RenderPass::addAttachment(std::initializer_list<Attachment> _attachments) {
			mAttachments->insert(_attachments.begin(), _attachments.end());
		}

		void RenderPass::addAttachment(const Attachment& _attachment) {
			mAttachments->emplace(_attachment);
		}

		void RenderPass::addSubpass(std::initializer_list<Subpass> _subpasses) {
			mSubpasses->insert(_subpasses.begin(), _subpasses.end());
		}

		void RenderPass::addSubpass(const Subpass& _subpass) {
			mSubpasses->emplace(_subpass);
		}

		void RenderPass::addDependency(const vk::SubpassDependency& _dependency) {
			mDependencies->emplace_back(_dependency);
		}

		void RenderPass::addDependency(std::initializer_list<vk::SubpassDependency> _dependencies) {
			mDependencies->insert(mDependencies->end(), _dependencies);
		}
	}
}

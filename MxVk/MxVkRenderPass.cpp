#include "MxVkRenderPass.h"

#include "vulkan/vulkan.h"
namespace Mix {
    namespace Graphics {
        void RenderPass::clear() {
            
        }

        void RenderPass::init(std::shared_ptr<Core>& _core) {
            mCore = _core;
            mAttachments = std::make_shared<std::vector<vk::AttachmentDescription>>();
            mAttachRefs = std::make_shared<std::vector<vk::AttachmentReference>>();
            mDependencies = std::make_shared<std::vector<vk::SubpassDependency>>();
            mSubpasses = std::make_shared<std::vector<SubpassContent>>();
        }

        ArrayIndex RenderPass::addColorAttach(vk::Format _format, vk::SampleCountFlagBits _sampleCount, vk::AttachmentLoadOp _loadOp, vk::AttachmentStoreOp _storeOp, vk::ImageLayout _initLayout, vk::ImageLayout _finalLayout) {
            vk::AttachmentDescription descri;
            descri.format = _format;
            descri.samples = _sampleCount;
            descri.loadOp = _loadOp;
            descri.storeOp = _storeOp;
            descri.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            descri.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            descri.initialLayout = _initLayout;
            descri.finalLayout = _finalLayout;
            mAttachments->push_back(std::move(descri));
            return mAttachments->size() - 1;
        }

        ArrayIndex RenderPass::addDepthStencilAttach(vk::Format _format, vk::SampleCountFlagBits _sampleCount, vk::AttachmentLoadOp _loadOp, vk::AttachmentStoreOp _storeOp, vk::AttachmentLoadOp _stencilLoadOp, vk::AttachmentStoreOp _stencilStoreOp, vk::ImageLayout _initLayout, vk::ImageLayout _finalLayout) {
            vk::AttachmentDescription descri;
            descri.format = _format;
            descri.samples = _sampleCount;
            descri.loadOp = _loadOp;
            descri.storeOp = _storeOp;
            descri.stencilLoadOp = _stencilLoadOp;
            descri.stencilStoreOp = _stencilStoreOp;
            descri.initialLayout = _initLayout;
            descri.finalLayout = _finalLayout;
            mAttachments->push_back(std::move(descri));
            return mAttachments->size() - 1;
        }

        ArrayIndex RenderPass::addColorAttachRef(ArrayIndex _index, vk::ImageLayout _layout) {
            vk::AttachmentReference ref;
            ref.attachment = static_cast<uint32_t>(_index);
            ref.layout = _layout;
            mAttachRefs->push_back(std::move(ref));

            return mAttachRefs->size() - 1;
        }

        ArrayIndex RenderPass::addDepthStencilAttachRef(ArrayIndex _index, vk::ImageLayout _layout) {
            vk::AttachmentReference ref;
            ref.attachment = static_cast<uint32_t>(_index);
            ref.layout = _layout;
            mAttachRefs->push_back(std::move(ref));
            return mAttachRefs->size() - 1;
        }

        ArrayIndex RenderPass::addSubpass(vk::PipelineBindPoint _bindPoint) {
            SubpassContent subpass;
            subpass.bindPoint = _bindPoint;
            mSubpasses->push_back(std::move(subpass));
            return mSubpasses->size() - 1;
        }

        bool RenderPass::addSubpassColorRef(ArrayIndex _subpassIndex, const std::vector<ArrayIndex>& _refIndices) {
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
        }

        bool RenderPass::addSubpassColorRef(ArrayIndex _subpassIndex, const ArrayIndex _refIndex) {
            if (!(_refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[_subpassIndex].colorRef.push_back((*mAttachRefs)[_refIndex]);
            return true;
        }

        bool RenderPass::addSubpassDepthStencilRef(ArrayIndex _subpassIndex, ArrayIndex _refIndex) {
            if (!(_refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[_subpassIndex].depthStencilRef = (*mAttachRefs)[_refIndex];
            (*mSubpasses)[_subpassIndex].hasDepthStencilRef = true;
            return true;
        }

        bool RenderPass::addSubpassResolveRef(ArrayIndex _subpassIndex, ArrayIndex _refIndex) {
            if (!(_refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[_subpassIndex].resolveRef = (*mAttachRefs)[_refIndex];
            (*mSubpasses)[_subpassIndex].hasResolveRef = true;
            return true;
        }

        ArrayIndex RenderPass::addDependency(ArrayIndex _srcSubpass, ArrayIndex _dstSubpass, vk::PipelineStageFlags _srcStage, vk::PipelineStageFlags _dstStage, vk::AccessFlags _srcAccess, vk::AccessFlags _dstAccess) {
            if (_srcSubpass != VK_SUBPASS_EXTERNAL) {
                if (_srcSubpass > _dstSubpass)
                    return ~0U;
                if (!(_srcSubpass < mSubpasses->size()))
                    return ~0U;
            }
            if (_dstSubpass != VK_SUBPASS_EXTERNAL && !(_dstSubpass > mSubpasses->size()))
                return ~0U;

            vk::SubpassDependency dependency;
            dependency.srcSubpass = static_cast<uint32_t>(_srcSubpass);
            dependency.dstSubpass = static_cast<uint32_t>(_dstSubpass);
            dependency.srcStageMask = _srcStage;
            dependency.dstStageMask = _dstStage;
            dependency.srcAccessMask = _srcAccess;
            dependency.dstAccessMask = _dstAccess;
            mDependencies->push_back(std::move(dependency));
            return mDependencies->size() - 1;
        }

        void RenderPass::create() {
            std::vector<vk::SubpassDescription> subpasses(mSubpasses->size());
            for (uint32_t i = 0; i < subpasses.size(); ++i) {
                subpasses[i].pipelineBindPoint = (*mSubpasses)[i].bindPoint;

                if ((*mSubpasses)[i].colorRef.size() > 0) {
                    subpasses[i].pColorAttachments = (*mSubpasses)[i].colorRef.data();
                    subpasses[i].colorAttachmentCount = static_cast<uint32_t>((*mSubpasses)[i].colorRef.size());
                }

                if ((*mSubpasses)[i].inputRef.size() > 0) {
                    subpasses[i].pInputAttachments = (*mSubpasses)[i].inputRef.data();
                    subpasses[i].inputAttachmentCount = static_cast<uint32_t>((*mSubpasses)[i].inputRef.size());
                }

                if ((*mSubpasses)[i].preserveRef.size() > 0) {
                    subpasses[i].pResolveAttachments = (*mSubpasses)[i].preserveRef.data();
                    subpasses[i].preserveAttachmentCount = static_cast<uint32_t>((*mSubpasses)[i].preserveRef.size());
                }

                if ((*mSubpasses)[i].hasDepthStencilRef) {
                    subpasses[i].pDepthStencilAttachment = &(*mSubpasses)[i].depthStencilRef;
                }

                if ((*mSubpasses)[i].hasResolveRef) {
                    subpasses[i].pResolveAttachments = &(*mSubpasses)[i].resolveRef;
                }
            }

            vk::RenderPassCreateInfo createInfo;
            createInfo.pAttachments = mAttachments->data();
            createInfo.attachmentCount = static_cast<uint32_t>(mAttachments->size());
            createInfo.pSubpasses = subpasses.data();
            createInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
            createInfo.pDependencies = mDependencies->data();
            createInfo.dependencyCount = static_cast<uint32_t>(mDependencies->size());

            mRenderPass = mCore->getDevice().createRenderPass(createInfo);
            clear();
        }

        void RenderPass::beginRenderPass(const vk::CommandBuffer& _cmdBuffer, const vk::Framebuffer& _frameBuffer, std::vector<vk::ClearValue>& _clearValues, const vk::Extent2D & _extent, const vk::Offset2D & _offset, const vk::SubpassContents _subpassContent) {
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
            if (!mCore)
                return;

            mCore->getDevice().destroyRenderPass(mRenderPass);
            mCore = nullptr;
            clear();
        }
    }
}
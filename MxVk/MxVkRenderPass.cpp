#include "MxVkRenderPass.h"

#include "vulkan/vulkan.h"
namespace Mix {
    namespace Graphics {
        void RenderPass::clear() {
            
        }

        void RenderPass::init(std::shared_ptr<Core>& core) {
            mCore = core;
            mAttachments = std::make_shared<std::vector<vk::AttachmentDescription>>();
            mAttachRefs = std::make_shared<std::vector<vk::AttachmentReference>>();
            mDependencies = std::make_shared<std::vector<vk::SubpassDependency>>();
            mSubpasses = std::make_shared<std::vector<SubpassContent>>();
        }

        ArrayIndex RenderPass::addColorAttach(vk::Format format, vk::SampleCountFlagBits sampleCount, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp, vk::ImageLayout initLayout, vk::ImageLayout finalLayout) {
            vk::AttachmentDescription descri;
            descri.format = format;
            descri.samples = sampleCount;
            descri.loadOp = loadOp;
            descri.storeOp = storeOp;
            descri.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            descri.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            descri.initialLayout = initLayout;
            descri.finalLayout = finalLayout;
            mAttachments->push_back(std::move(descri));
            return mAttachments->size() - 1;
        }

        ArrayIndex RenderPass::addDepthStencilAttach(vk::Format format, vk::SampleCountFlagBits sampleCount, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp, vk::AttachmentLoadOp stencilLoadOp, vk::AttachmentStoreOp stencilStoreOp, vk::ImageLayout initLayout, vk::ImageLayout finalLayout) {
            vk::AttachmentDescription descri;
            descri.format = format;
            descri.samples = sampleCount;
            descri.loadOp = loadOp;
            descri.storeOp = storeOp;
            descri.stencilLoadOp = stencilLoadOp;
            descri.stencilStoreOp = stencilStoreOp;
            descri.initialLayout = initLayout;
            descri.finalLayout = finalLayout;
            mAttachments->push_back(std::move(descri));
            return mAttachments->size() - 1;
        }

        ArrayIndex RenderPass::addColorAttachRef(ArrayIndex index, vk::ImageLayout layout) {
            vk::AttachmentReference ref;
            ref.attachment = static_cast<uint32_t>(index);
            ref.layout = layout;
            mAttachRefs->push_back(std::move(ref));

            return mAttachRefs->size() - 1;
        }

        ArrayIndex RenderPass::addDepthStencilAttachRef(ArrayIndex index, vk::ImageLayout layout) {
            vk::AttachmentReference ref;
            ref.attachment = static_cast<uint32_t>(index);
            ref.layout = layout;
            mAttachRefs->push_back(std::move(ref));
            return mAttachRefs->size() - 1;
        }

        ArrayIndex RenderPass::addSubpass(vk::PipelineBindPoint bindPoint) {
            SubpassContent subpass;
            subpass.bindPoint = bindPoint;
            mSubpasses->push_back(std::move(subpass));
            return mSubpasses->size() - 1;
        }

        bool RenderPass::addSubpassColorRef(ArrayIndex subpassIndex, const std::vector<ArrayIndex>& refIndices) {
            for (auto& index : refIndices) {
                if (!(index < mAttachRefs->size()))
                    return false;
            }
            auto& subpass = (*mSubpasses)[subpassIndex];
            subpass.colorRef.reserve(subpass.colorRef.size() + refIndices.size());
            for (const auto& index : refIndices) {
                subpass.colorRef.push_back((*mAttachRefs)[index]);
            }
            return true;
        }

        bool RenderPass::addSubpassColorRef(ArrayIndex subpassIndex, const ArrayIndex refIndex) {
            if (!(refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[subpassIndex].colorRef.push_back((*mAttachRefs)[refIndex]);
            return true;
        }

        bool RenderPass::addSubpassDepthStencilRef(ArrayIndex subpassIndex, ArrayIndex refIndex) {
            if (!(refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[subpassIndex].depthStencilRef = (*mAttachRefs)[refIndex];
            (*mSubpasses)[subpassIndex].hasDepthStencilRef = true;
            return true;
        }

        bool RenderPass::addSubpassResolveRef(ArrayIndex subpassIndex, ArrayIndex refIndex) {
            if (!(refIndex < mAttachRefs->size()))
                return false;
            (*mSubpasses)[subpassIndex].resolveRef = (*mAttachRefs)[refIndex];
            (*mSubpasses)[subpassIndex].hasResolveRef = true;
            return true;
        }

        ArrayIndex RenderPass::addDependency(ArrayIndex srcSubpass, ArrayIndex dstSubpass, vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage, vk::AccessFlags srcAccess, vk::AccessFlags dstAccess) {
            if (srcSubpass != VK_SUBPASS_EXTERNAL) {
                if (srcSubpass > dstSubpass)
                    return ~0U;
                if (!(srcSubpass < mSubpasses->size()))
                    return ~0U;
            }
            if (dstSubpass != VK_SUBPASS_EXTERNAL && !(dstSubpass > mSubpasses->size()))
                return ~0U;

            vk::SubpassDependency dependency;
            dependency.srcSubpass = static_cast<uint32_t>(srcSubpass);
            dependency.dstSubpass = static_cast<uint32_t>(dstSubpass);
            dependency.srcStageMask = srcStage;
            dependency.dstStageMask = dstStage;
            dependency.srcAccessMask = srcAccess;
            dependency.dstAccessMask = dstAccess;
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

            mRenderPass = mCore->device().createRenderPass(createInfo);
            clear();
        }

        void RenderPass::beginRenderPass(const vk::CommandBuffer& cmdBuffer, const vk::Framebuffer& frameBuffer, std::vector<vk::ClearValue>& clearValues, const vk::Extent2D & extent, const vk::Offset2D & offset, const vk::SubpassContents subpassContent) {
            vk::RenderPassBeginInfo beginInfo;
            beginInfo.renderPass = mRenderPass;
            beginInfo.framebuffer = frameBuffer;
            beginInfo.pClearValues = clearValues.data();
            beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            beginInfo.renderArea.offset = offset;
            beginInfo.renderArea.extent = extent;

            cmdBuffer.beginRenderPass(beginInfo, subpassContent);
        }

        void RenderPass::endRenderPass(const vk::CommandBuffer& cmdBuffer) {
            cmdBuffer.endRenderPass();
        }

        void RenderPass::destroy() {
            if (!mCore)
                return;

            mCore->device().destroyRenderPass(mRenderPass);
            mCore = nullptr;
            clear();
        }
    }
}
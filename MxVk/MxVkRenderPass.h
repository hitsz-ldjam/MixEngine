#pragma once
#ifndef _MX_VK_RENDERPASS_H_
#define _MX_VK_RENDERPASS_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class RenderPass :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            RenderPass() { mCore = nullptr; }
            virtual ~RenderPass() { destroy(); }

            void init(const Core* core) override;

            void create();

            const vk::RenderPass& get() const { return mRenderPass; }

            void beginRenderPass(const vk::CommandBuffer& cmdBuffer,
                                 const vk::Framebuffer& frameBuffer,
                                 std::vector<vk::ClearValue>& clearValues,
                                 const vk::Extent2D& extent,
                                 const vk::Offset2D& offset = { 0,0 },
                                 const vk::SubpassContents subpassContent = vk::SubpassContents::eInline);

            void endRenderPass(const vk::CommandBuffer& cmdBuffer);

            void destroy();



            ArrayIndex addColorAttach(vk::Format format, vk::SampleCountFlagBits sampleCount,
                                      vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,
                                      vk::ImageLayout initLayout, vk::ImageLayout finalLayout);

            ArrayIndex addDepthStencilAttach(vk::Format format,
                                             vk::SampleCountFlagBits sampleCount,
                                             vk::AttachmentLoadOp loadOp = vk::AttachmentLoadOp::eClear,
                                             vk::AttachmentStoreOp storeOp = vk::AttachmentStoreOp::eDontCare,
                                             vk::AttachmentLoadOp stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                             vk::AttachmentStoreOp stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                             vk::ImageLayout initLayout = vk::ImageLayout::eUndefined,
                                             vk::ImageLayout finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addColorAttachRef(ArrayIndex index,
                                         vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal);

            ArrayIndex addDepthStencilAttachRef(ArrayIndex index,
                                                vk::ImageLayout layout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addSubpass(vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics);

            bool addSubpassColorRef(ArrayIndex subpassIndex, const std::vector<ArrayIndex>& refIndices);

            bool addSubpassColorRef(ArrayIndex subpassIndex, const ArrayIndex refIndex);

            bool addSubpassDepthStencilRef(ArrayIndex subpassIndex, ArrayIndex refIndex);

            bool addSubpassResolveRef(ArrayIndex subpassIndex, ArrayIndex refIndex);

            ArrayIndex addDependency(ArrayIndex srcSubpass,
                                     ArrayIndex dstSubpass,
                                     vk::PipelineStageFlags srcStage,
                                     vk::PipelineStageFlags dstStage,
                                     vk::AccessFlags srcAccess,
                                     vk::AccessFlags dstAccess);

        private:
            vk::RenderPass mRenderPass;

            struct SubpassContent {
                vk::PipelineBindPoint bindPoint;
                std::vector<vk::AttachmentReference> colorRef;
                std::vector<vk::AttachmentReference> inputRef;
                std::vector<vk::AttachmentReference> preserveRef;
                bool hasDepthStencilRef = false;
                vk::AttachmentReference depthStencilRef;
                bool hasResolveRef = false;
                vk::AttachmentReference resolveRef;
            };

            std::vector<vk::AttachmentDescription>* mAttachments;
            std::vector<vk::AttachmentReference>* mAttachRefs;
            std::vector<vk::SubpassDependency>* mDependencies;
            std::vector<SubpassContent>* mSubpasses;

            void clear();

        };
    }
}
#endif // !_MX_VK_RENDERPASS_H_

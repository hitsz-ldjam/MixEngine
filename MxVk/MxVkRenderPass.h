#pragma once
#ifndef MX_VK_RENDERPASS_H_
#define MX_VK_RENDERPASS_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class RenderPass :public GraphicsComponent {
        public:
            RenderPass() { mCore = nullptr; }
            ~RenderPass() { destroy(); }

            void init(std::shared_ptr<Core>& _core);

            void create();

            const vk::RenderPass& get() const { return mRenderPass; }

            void beginRenderPass(const vk::CommandBuffer& _cmdBuffer,
                                 const vk::Framebuffer& _frameBuffer,
                                 std::vector<vk::ClearValue>& _clearValues,
                                 const vk::Extent2D& _extent,
                                 const vk::Offset2D& _offset = { 0,0 },
                                 const vk::SubpassContents _subpassContent = vk::SubpassContents::eInline);

            void endRenderPass(const vk::CommandBuffer& _cmdBuffer);

            void destroy();



            ArrayIndex addColorAttach(vk::Format _format, 
                                      vk::SampleCountFlagBits _sampleCount,
                                      vk::AttachmentLoadOp _loadOp, 
                                      vk::AttachmentStoreOp _storeOp,
                                      vk::ImageLayout _initLayout, 
                                      vk::ImageLayout _finalLayout);

            ArrayIndex addDepthStencilAttach(vk::Format _format,
                                             vk::SampleCountFlagBits _sampleCount,
                                             vk::AttachmentLoadOp _loadOp = vk::AttachmentLoadOp::eClear,
                                             vk::AttachmentStoreOp _storeOp = vk::AttachmentStoreOp::eDontCare,
                                             vk::AttachmentLoadOp _stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                             vk::AttachmentStoreOp _stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                             vk::ImageLayout _initLayout = vk::ImageLayout::eUndefined,
                                             vk::ImageLayout _finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addColorAttachRef(ArrayIndex _index,
                                         vk::ImageLayout _layout = vk::ImageLayout::eColorAttachmentOptimal);

            ArrayIndex addDepthStencilAttachRef(ArrayIndex _index,
                                                vk::ImageLayout _layout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addSubpass(vk::PipelineBindPoint _bindPoint = vk::PipelineBindPoint::eGraphics);

            bool addSubpassColorRef(ArrayIndex _subpassIndex, const std::vector<ArrayIndex>& _refIndices);

            bool addSubpassColorRef(ArrayIndex _subpassIndex, const ArrayIndex _refIndex);

            bool addSubpassDepthStencilRef(ArrayIndex _subpassIndex, ArrayIndex _refIndex);

            bool addSubpassResolveRef(ArrayIndex _subpassIndex, ArrayIndex _refIndex);

            ArrayIndex addDependency(ArrayIndex _srcSubpass,
                                     ArrayIndex _dstSubpass,
                                     vk::PipelineStageFlags _srcStage,
                                     vk::PipelineStageFlags _dstStage,
                                     vk::AccessFlags _srcAccess,
                                     vk::AccessFlags _dstAccess);

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

            std::shared_ptr<std::vector<vk::AttachmentDescription>> mAttachments;
            std::shared_ptr<std::vector<vk::AttachmentReference>> mAttachRefs;
            std::shared_ptr<std::vector<vk::SubpassDependency>> mDependencies;
            std::shared_ptr<std::vector<SubpassContent>> mSubpasses;

            void clear();

        };
    }
}
#endif // !MX_VK_RENDERPASS_H_

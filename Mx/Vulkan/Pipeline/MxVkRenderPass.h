#pragma once
#ifndef MX_VK_RENDERPASS_H_
#define MX_VK_RENDERPASS_H_


#include "../Core/MxVkCore.h"


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



            ArrayIndex addColorAttach(const vk::Format _format, 
                                      const vk::SampleCountFlagBits _sampleCount,
                                      const vk::AttachmentLoadOp _loadOp, 
                                      const vk::AttachmentStoreOp _storeOp,
                                      const vk::ImageLayout _initLayout, 
                                      const vk::ImageLayout _finalLayout);

            ArrayIndex addDepthStencilAttach(const vk::Format _format,
                                             const vk::SampleCountFlagBits _sampleCount,
                                             const vk::AttachmentLoadOp _loadOp = vk::AttachmentLoadOp::eClear,
                                             const vk::AttachmentStoreOp _storeOp = vk::AttachmentStoreOp::eDontCare,
                                             const vk::AttachmentLoadOp _stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
                                             const vk::AttachmentStoreOp _stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
                                             const vk::ImageLayout _initLayout = vk::ImageLayout::eUndefined,
                                             const vk::ImageLayout _finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addColorAttachRef(const ArrayIndex _index,
                                         const vk::ImageLayout _layout = vk::ImageLayout::eColorAttachmentOptimal);

            ArrayIndex addDepthStencilAttachRef(const ArrayIndex _index,
                                                const vk::ImageLayout _layout = vk::ImageLayout::eDepthStencilAttachmentOptimal);

            ArrayIndex addSubpass(const vk::PipelineBindPoint _bindPoint = vk::PipelineBindPoint::eGraphics);

            bool addSubpassColorRef(const ArrayIndex _subpassIndex, 
                                    const std::vector<ArrayIndex>& _refIndices);

            bool addSubpassColorRef(const ArrayIndex _subpassIndex, 
                                    const ArrayIndex _refIndex);

            bool addSubpassDepthStencilRef(const ArrayIndex _subpassIndex, 
                                           const ArrayIndex _refIndex);

            bool addSubpassResolveRef(ArrayIndex _subpassIndex, ArrayIndex _refIndex);

            ArrayIndex addDependency(const ArrayIndex _srcSubpass,
                                     const ArrayIndex _dstSubpass,
                                     const vk::PipelineStageFlags& _srcStage,
                                     const vk::PipelineStageFlags& _dstStage,
                                     const vk::AccessFlags& _srcAccess,
                                     const vk::AccessFlags& _dstAccess);

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

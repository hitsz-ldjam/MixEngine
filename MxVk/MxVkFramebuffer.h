#pragma once
#ifndef MX_VK_FRAMEBUFFER_H_
#define MX_VK_FRAMEBUFFER_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class Framebuffer :public GraphicsComponent {
            using Base = GraphicsComponent;
        public:
            Framebuffer() = default;

            Framebuffer(const Framebuffer& _other) = default;

            Framebuffer(Framebuffer&& _other) noexcept = default;

            Framebuffer& operator=(const Framebuffer& _other) = default;

            Framebuffer& operator=(Framebuffer&& _other) noexcept = default;

            ~Framebuffer() { destroy(); }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            void setTargetRenderPass(const vk::RenderPass _renderPass) { mRenderPass = _renderPass; }

            void setExtent(const vk::Extent2D& _extent) { mExtent = _extent; }

            void setLayers(const uint32_t _layer) { mLayers = _layer; }

            void addAttachments(const std::vector<vk::ImageView>& _attachments);

            void create();

            vk::Framebuffer get() const { return mFramebuffer; };

            void destroy();

        private:
            vk::Framebuffer mFramebuffer;
            vk::RenderPass mRenderPass;
            vk::Extent2D mExtent;
            uint32_t mLayers = 0;

            std::vector<vk::ImageView> mAttachments;
            void clear();
        };
    }
}
#endif // !MX_VK_FRAMEBUFFER_H_

#pragma once
#ifndef _MX_VK_FRAMEBUFFER_H_
#define _MX_VK_FRAMEBUFFER_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class Framebuffer :public GraphicsComponent {
        public:
            virtual ~Framebuffer() { destroy(); }

            void setTargetRenderPass(const vk::RenderPass renderPass) { mRenderPass = renderPass; };
            void setExtent(const vk::Extent2D& extent) { mExtent = extent; };
            void setLayers(const uint32_t layer) { mLayers = layer; };
            void addAttachments(const std::vector<vk::ImageView>& attachments);
            void create();
            vk::Framebuffer get() const { return mFramebuffer; };
            void destroy();

        private:
            vk::Framebuffer mFramebuffer;
            vk::RenderPass mRenderPass;
            vk::Extent2D mExtent;
            uint32_t mLayers;

            std::vector<vk::ImageView> mAttachments;
            void clear();
        };
    }
}
#endif // !_MX_VK_FRAMEBUFFER_H_

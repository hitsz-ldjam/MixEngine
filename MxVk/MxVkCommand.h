#pragma once
#ifndef _MX_VK_COMMAND_H_
#define _MX_VK_COMMAND_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class CommandMgr :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~CommandMgr() { destroy(); }
            void create(const vk::QueueFlagBits queueType);

            std::vector<vk::CommandBuffer> allocCommandBuffers(vk::CommandBufferLevel level, uint32_t count);

            void freeCommandBuffers(const std::vector<vk::CommandBuffer>& commandBuffers);

            vk::CommandBuffer beginTempCommandBuffer();

            void endTempCommandBuffer(vk::CommandBuffer commandBuffer);

            void destroy();

        private:
            vk::Queue mQueue;

            vk::CommandPool mCommandPool;
            std::vector<vk::CommandBuffer> mCommandBuffers;

            static vk::CommandBufferAllocateInfo sTempBufferAllocInfo;
            static vk::CommandBufferBeginInfo sTempBufferBeginInfo;
            static vk::SubmitInfo sTempBufferSubmitInfo;
        };
    }
}
#endif // !_MX_VK_COMMAND_H_

#pragma once
#ifndef _MX_VK_COMMAND_H_
#define _MX_VK_COMMAND_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class CommandMgr :public GraphicsComponent {
        public:
            virtual ~CommandMgr() { destroy(); }
            void create(const vk::QueueFlagBits queueType,
                        const vk::CommandPoolCreateFlags& flags = vk::CommandPoolCreateFlags());

            std::vector<vk::CommandBuffer> allocCommandBuffers(uint32_t count, vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
            vk::CommandBuffer allocCommandBuffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
            void freeCommandBuffers(const std::vector<vk::CommandBuffer>& commandBuffers);
            void freeCommandBuffers(const vk::CommandBuffer commandBuffer);
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

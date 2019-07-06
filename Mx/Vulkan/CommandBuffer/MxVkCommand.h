//#pragma once
//#ifndef MX_VK_COMMAND_H_
//#define MX_VK_COMMAND_H_
//
//#include "../Core/MxVkCore.h"
//
//namespace Mix {
//    namespace Graphics {
//        class CommandMgr :public GraphicsComponent {
//        public:
//            ~CommandMgr() { destroy(); }
//
//            void init(const std::shared_ptr<Core>& _core) {
//                setCore(_core);
//            }
//
//            void create(const vk::QueueFlagBits _queueType,
//                        const vk::CommandPoolCreateFlags& _flags = vk::CommandPoolCreateFlags());
//
//            std::vector<vk::CommandBuffer> allocCommandBuffers(uint32_t _count, 
//                                                               vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);
//
//            vk::CommandBuffer allocCommandBuffer(vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);
//
//            void freeCommandBuffers(const std::vector<vk::CommandBuffer>& _commandBuffers) const;
//
//            void freeCommandBuffers(const vk::CommandBuffer& _commandBuffer);
//
//            vk::CommandBuffer beginTempCommandBuffer();
//
//            void endTempCommandBuffer(const vk::CommandBuffer& _commandBuffer) const;
//
//            void destroy();
//
//        private:
//            vk::Queue mQueue;
//
//            vk::CommandPool mCommandPool;
//            std::vector<vk::CommandBuffer> mCommandBuffers;
//
//            static vk::CommandBufferAllocateInfo TempBufferAllocInfo;
//            static vk::CommandBufferBeginInfo TempBufferBeginInfo;
//            static vk::SubmitInfo TempBufferSubmitInfo;
//        };
//    }
//}
//#endif // !MX_VK_COMMAND_H_

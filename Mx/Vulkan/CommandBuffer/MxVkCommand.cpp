//#include "MxVkCommand.h"
//
//namespace Mix {
//    namespace Graphics {
//        vk::CommandBufferAllocateInfo CommandMgr::TempBufferAllocInfo(vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1);
//        vk::CommandBufferBeginInfo CommandMgr::TempBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
//        vk::SubmitInfo CommandMgr::TempBufferSubmitInfo;
//
//        void CommandMgr::create(const vk::QueueFlagBits _queueType, const vk::CommandPoolCreateFlags& _flags) {
//            vk::CommandPoolCreateInfo createInfo = {};
//			
//            if (_queueType == vk::QueueFlagBits::eGraphics) {
//                createInfo.queueFamilyIndex = mCore->getQueueFamilyIndices().graphics.value();
//                mQueue = mCore->getQueues().graphics.value();
//            }
//            if (_queueType == vk::QueueFlagBits::eCompute) {
//                createInfo.queueFamilyIndex = mCore->getQueueFamilyIndices().compute.value();
//                mQueue = mCore->getQueues().compute.value();
//            }
//            createInfo.flags = _flags;
//
//            mCommandPool = mCore->getDevice().createCommandPool(createInfo);
//        }
//
//        std::vector<vk::CommandBuffer> CommandMgr::allocCommandBuffers(uint32_t _count, vk::CommandBufferLevel _level) {
//            vk::CommandBufferAllocateInfo allocateInfo = {};
//            allocateInfo.commandPool = mCommandPool;
//            allocateInfo.level = _level;
//            allocateInfo.commandBufferCount = _count;
//
//            std::vector<vk::CommandBuffer> buffer = mCore->getDevice().allocateCommandBuffers(allocateInfo);
//            mCommandBuffers.insert(mCommandBuffers.end(), buffer.cbegin(), buffer.cend());
//            return buffer;
//
//        }
//
//        vk::CommandBuffer CommandMgr::allocCommandBuffer(vk::CommandBufferLevel _level) {
//            return allocCommandBuffers(1, _level)[0];
//        }
//
//        void CommandMgr::freeCommandBuffers(const std::vector<vk::CommandBuffer>& _commandBuffers) const {
//            for (const auto& buffer : _commandBuffers) {
//                if (std::find(mCommandBuffers.cbegin(), mCommandBuffers.cend(), buffer) == mCommandBuffers.cend())
//                    throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
//            }
//            mCore->getDevice().freeCommandBuffers(mCommandPool, _commandBuffers);
//        }
//
//        void CommandMgr::freeCommandBuffers(const vk::CommandBuffer& _commandBuffer) {
//            auto it = std::find(mCommandBuffers.begin(), mCommandBuffers.end(), _commandBuffer);
//            if (it == mCommandBuffers.end())
//                throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
//            mCore->getDevice().freeCommandBuffers(mCommandPool, _commandBuffer);
//
//        }
//
//        vk::CommandBuffer CommandMgr::beginTempCommandBuffer() {
//            TempBufferAllocInfo.commandPool = mCommandPool;
//            vk::CommandBuffer temp = mCore->getDevice().allocateCommandBuffers(TempBufferAllocInfo)[0];
//            temp.begin(TempBufferBeginInfo);
//            return temp;
//        }
//
//        void CommandMgr::endTempCommandBuffer(const vk::CommandBuffer& _commandBuffer) const {
//            _commandBuffer.end();
//            TempBufferSubmitInfo.pCommandBuffers = &_commandBuffer;
//            TempBufferSubmitInfo.commandBufferCount = 1;
//            mQueue.submit(TempBufferSubmitInfo, nullptr);
//            mQueue.waitIdle();
//        }
//
//        void CommandMgr::destroy() {
//            if (!mCore)
//                return;
//
//            mCore->getDevice().destroyCommandPool(mCommandPool);
//            mCommandPool = nullptr;
//            mCommandBuffers.clear();
//            mCore = nullptr;
//        }
//    }
//}
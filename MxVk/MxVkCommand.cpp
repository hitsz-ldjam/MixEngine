#include "MxVkCommand.h"

namespace Mix {
    namespace Graphics {
        vk::CommandBufferAllocateInfo CommandMgr::sTempBufferAllocInfo(vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1);
        vk::CommandBufferBeginInfo CommandMgr::sTempBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vk::SubmitInfo CommandMgr::sTempBufferSubmitInfo;

        void CommandMgr::create(const vk::QueueFlagBits queueType, const vk::CommandPoolCreateFlags& flags) {
            vk::CommandPoolCreateInfo createInfo = {};

            if (queueType == vk::QueueFlagBits::eGraphics) {
                createInfo.queueFamilyIndex = mCore->GetQueueFamilyIndices().graphics.value();
                mQueue = mCore->GetQueues().graphics.value();
            }
            if (queueType == vk::QueueFlagBits::eCompute) {
                createInfo.queueFamilyIndex = mCore->GetQueueFamilyIndices().compute.value();
                mQueue = mCore->GetQueues().compute.value();
            }
            createInfo.flags = flags;

            mCommandPool = mCore->GetDevice().createCommandPool(createInfo);
        }

        std::vector<vk::CommandBuffer> CommandMgr::allocCommandBuffers(uint32_t count, vk::CommandBufferLevel level) {
            vk::CommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.commandPool = mCommandPool;
            allocateInfo.level = level;
            allocateInfo.commandBufferCount = count;

            std::vector<vk::CommandBuffer> buffer = mCore->GetDevice().allocateCommandBuffers(allocateInfo);
            mCommandBuffers.insert(mCommandBuffers.end(), buffer.cbegin(), buffer.cend());
            return std::move(buffer);

        }

        vk::CommandBuffer CommandMgr::allocCommandBuffer(vk::CommandBufferLevel level) {
            return allocCommandBuffers(1, level)[0];
        }

        void CommandMgr::freeCommandBuffers(const std::vector<vk::CommandBuffer>& commandBuffers) {
            for (const auto& buffer : commandBuffers) {
                if (std::find(mCommandBuffers.cbegin(), mCommandBuffers.cend(), buffer) == mCommandBuffers.cend())
                    throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
            }
            mCore->GetDevice().freeCommandBuffers(mCommandPool, commandBuffers);
        }

        void CommandMgr::freeCommandBuffers(const vk::CommandBuffer commandBuffer) {
            auto it = std::find(mCommandBuffers.begin(), mCommandBuffers.end(), commandBuffer);
            if(it==mCommandBuffers.end())
                throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
            mCore->GetDevice().freeCommandBuffers(mCommandPool, commandBuffer);

        }

        vk::CommandBuffer CommandMgr::beginTempCommandBuffer() {
            sTempBufferAllocInfo.commandPool = mCommandPool;
            vk::CommandBuffer temp = mCore->GetDevice().allocateCommandBuffers(sTempBufferAllocInfo)[0];
            temp.begin(sTempBufferBeginInfo);
            return temp;
        }

        void CommandMgr::endTempCommandBuffer(vk::CommandBuffer commandBuffer) {
            commandBuffer.end();
            sTempBufferSubmitInfo.pCommandBuffers = &commandBuffer;
            sTempBufferSubmitInfo.commandBufferCount = 1;
            mQueue.submit(sTempBufferSubmitInfo, nullptr);
            mQueue.waitIdle();
        }

        void CommandMgr::destroy() {
            if (!mCore)
                return;

            mCore->GetDevice().destroyCommandPool(mCommandPool);
            mCommandPool = nullptr;
            mCommandBuffers.clear();
            mCore = nullptr;
        }
    }
}
#include "MxVkCommand.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(CommandMgr, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(CommandMgr);

        vk::CommandBufferAllocateInfo CommandMgr::sTempBufferAllocInfo(vk::CommandPool(), vk::CommandBufferLevel::ePrimary, 1);
        vk::CommandBufferBeginInfo CommandMgr::sTempBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        vk::SubmitInfo CommandMgr::sTempBufferSubmitInfo;

        void CommandMgr::create(const vk::QueueFlagBits queueType) {
            vk::CommandPoolCreateInfo createInfo = {};

            if (queueType == vk::QueueFlagBits::eGraphics) {
                createInfo.queueFamilyIndex = mCore->getQueueFamilyIndices().graphics.value();
                mQueue = mCore->getQueues().graphics.value();
            }
            if (queueType == vk::QueueFlagBits::eCompute) {
                createInfo.queueFamilyIndex = mCore->getQueueFamilyIndices().compute.value();
                mQueue = mCore->getQueues().compute.value();
            }

            mCommandPool = mCore->device().createCommandPool(createInfo);
        }

        std::vector<vk::CommandBuffer> CommandMgr::allocCommandBuffers(uint32_t count, vk::CommandBufferLevel level) {
            vk::CommandBufferAllocateInfo allocateInfo = {};
            allocateInfo.commandPool = mCommandPool;
            allocateInfo.level = level;
            allocateInfo.commandBufferCount = count;

            std::vector<vk::CommandBuffer> buffer = mCore->device().allocateCommandBuffers(allocateInfo);
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
            mCore->device().freeCommandBuffers(mCommandPool, commandBuffers);
        }

        void CommandMgr::freeCommandBuffers(const vk::CommandBuffer commandBuffer) {
            auto it = std::find(mCommandBuffers.begin(), mCommandBuffers.end(), commandBuffer);
            if(it==mCommandBuffers.end())
                throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
            mCore->device().freeCommandBuffers(mCommandPool, commandBuffer);

        }

        vk::CommandBuffer CommandMgr::beginTempCommandBuffer() {
            sTempBufferAllocInfo.commandPool = mCommandPool;
            vk::CommandBuffer temp = mCore->device().allocateCommandBuffers(sTempBufferAllocInfo)[0];
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

            mCore->device().destroyCommandPool(mCommandPool);
            mCommandPool = nullptr;
            mCommandBuffers.clear();
            mCore = nullptr;
        }
    }
}
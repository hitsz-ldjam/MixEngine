#include "MxVkCommandPool.h"

namespace Mix {
	namespace Graphics {

		CommandPool::CommandPool(const std::shared_ptr<Device>& _device,
											 const vk::QueueFlagBits _queueType,
											 const vk::CommandPoolCreateFlags& _flags)
			: mDevice(_device) {
			vk::CommandPoolCreateInfo createInfo;

			if (_queueType == vk::QueueFlagBits::eGraphics) {
				createInfo.queueFamilyIndex = mDevice->getQueueFamilyIndexSet().graphics.value();
				mQueue = mDevice->getQueueSet().graphics.value();
			}
			else if (_queueType == vk::QueueFlagBits::eCompute) {
				createInfo.queueFamilyIndex = mDevice->getQueueFamilyIndexSet().compute.value();
				mQueue = mDevice->getQueueSet().compute.value();
			}
			else if (_queueType == vk::QueueFlagBits::eTransfer) {
				createInfo.queueFamilyIndex = mDevice->getQueueFamilyIndexSet().transfer.value();
				mQueue = mDevice->getQueueSet().transfer.value();
			}

			createInfo.flags = _flags;
			mCommandPool = mDevice->get().createCommandPoolUnique(createInfo);
		}

		void CommandPool::swap(CommandPool& _other) noexcept {
			using std::swap;
			swap(mCommandPool, _other.mCommandPool);
			swap(mQueue, _other.mQueue);
			swap(mDevice, _other.mDevice);
		}

		vk::CommandBuffer CommandPool::allocate(const vk::CommandBufferLevel _level) {
			return mDevice->get().allocateCommandBuffers(vk::CommandBufferAllocateInfo(*mCommandPool, _level, 1))[0];
		}

		std::vector<vk::CommandBuffer> CommandPool::allocate(const uint32_t _count, const vk::CommandBufferLevel _level) {
			return mDevice->get().allocateCommandBuffers(vk::CommandBufferAllocateInfo(*mCommandPool, _level, _count));
		}

		vk::UniqueCommandBuffer CommandPool::allocateUnique(const vk::CommandBufferLevel _level) {
			return std::move(mDevice->get().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(*mCommandPool, _level, 1))[0]);
		}

		std::vector<vk::UniqueCommandBuffer> CommandPool::allocateUnique(const uint32_t _count,
		                                                                       const vk::CommandBufferLevel _level) {
			return mDevice->get().allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(*mCommandPool, _level, _count));
		}

		void CommandPool::free(const vk::CommandBuffer& _cmdBuffer) {
			mDevice->get().freeCommandBuffers(*mCommandPool, _cmdBuffer);
		}

		void CommandPool::free(const std::vector<vk::CommandBuffer>& _cmdBuffers) {
			mDevice->get().freeCommandBuffers(*mCommandPool, _cmdBuffers);
		}
	}
}

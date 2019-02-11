#include "MxVkCommandPool.h"
namespace Mix
{
	MxVkCommandPool::MxVkCommandPool() :mCommandPool(VK_NULL_HANDLE)
	{
	}

	VkCommandBufferAllocateInfo MxVkCommandPool::sTempBufferAllocInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr, VK_NULL_HANDLE, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1
	};

	VkCommandBufferBeginInfo MxVkCommandPool::sTempBufferBeginInfo = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr,
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr
	};

	bool MxVkCommandPool::createCommandPool(VkQueueFlagBits queueType)
	{
		if (!mIsReady)
			return false;

		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		if (queueType == VK_QUEUE_GRAPHICS_BIT)
		{
			createInfo.queueFamilyIndex = mManager->getQueueFamilyIndices().graphics;
			mQueue = mManager->getQueue().graphics;
		}
		if (queueType == VK_QUEUE_COMPUTE_BIT)
		{
			createInfo.queueFamilyIndex = mManager->getQueueFamilyIndices().compute;
			mQueue = mManager->getQueue().compute;
		}

		createInfo.flags = 0;
		MX_VK_CHECK_RESULT(vkCreateCommandPool(mManager->getDevice(), &createInfo, nullptr, &mCommandPool));
		return true;
	}

	std::vector<VkCommandBuffer> MxVkCommandPool::allocCommandBuffers(VkCommandBufferLevel level, uint32_t count)
	{
		VkCommandBufferAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = mCommandPool;
		allocateInfo.level = level;
		allocateInfo.commandBufferCount = count;

		std::vector<VkCommandBuffer> buffer;
		buffer.resize(count);
		MX_VK_CHECK_RESULT(vkAllocateCommandBuffers(mManager->getDevice(), &allocateInfo, buffer.data()));
		mCommandBuffers.insert(mCommandBuffers.end(), buffer.cbegin(), buffer.cend());
		return std::move(buffer);
	}

	bool beginCommandBuffer(const VkCommandBuffer commandBuffer, VkCommandBufferUsageFlagBits usage)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = usage;

		MX_VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		return true;
	}

	bool endCommandBuffer(const VkCommandBuffer commandBuffer)
	{
		MX_VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
		return true;
	}

	void MxVkCommandPool::freeCommandBuffers(const std::vector<VkCommandBuffer>& commandBuffers)
	{
		for (const auto& buffer : commandBuffers)
		{
			if (std::find(mCommandBuffers.cbegin(), mCommandBuffers.cend(), buffer) == mCommandBuffers.cend())
				throw std::runtime_error("Error : Member of [ commandBuffers ] not included in this command pool");
		}
		vkFreeCommandBuffers(mManager->getDevice(), mCommandPool, commandBuffers.size(), commandBuffers.data());
	}

	VkCommandBuffer MxVkCommandPool::beginTempCommandBuffer()
	{
		sTempBufferAllocInfo.commandPool = mCommandPool;
		VkCommandBuffer temp;

		MX_VK_CHECK_RESULT(vkAllocateCommandBuffers(mManager->getDevice(), &sTempBufferAllocInfo, &temp));

		MX_VK_CHECK_RESULT(vkBeginCommandBuffer(temp, &sTempBufferBeginInfo));
		return temp;
	}

	void MxVkCommandPool::endTempCommandBuffer(VkCommandBuffer commandBuffer)
	{
		MX_VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.commandBufferCount = 1;
		vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mQueue);
	}

	void MxVkCommandPool::destroy()
	{
		if (!mIsReady)
			return;

		vkDestroyCommandPool(mManager->getDevice(), mCommandPool, nullptr);
		mCommandPool = VK_NULL_HANDLE;
		mCommandBuffers.clear();
		mManager = nullptr;
		mIsReady = false;
	}

	MxVkCommandPool::~MxVkCommandPool()
	{
		destroy();
	}
}

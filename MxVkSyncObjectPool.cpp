#include "MxVkSyncObjectPool.h"

namespace Mix
{
	MxVkSyncObjectPool::MxVkSyncObjectPool()
	{
	}

	VkFence MxVkSyncObjectPool::createFence(const VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = flags;

		VkFence tempFence;
		MX_VK_CHECK_RESULT(vkCreateFence(mManager->getDevice(), &createInfo, nullptr, &tempFence));
		mFences.push_back(tempFence);
		return tempFence;
	}

	void MxVkSyncObjectPool::destroyFence(const VkFence fence)
	{
		auto it = std::find(mFences.cbegin(), mFences.cend(), fence);
		if (it == mFences.cend())
			throw std::runtime_error("Error : Can not find fence");

		vkDestroyFence(mManager->getDevice(), fence, nullptr);
		mFences.erase(it);
	}

	VkSemaphore MxVkSyncObjectPool::createSemaphore()
	{
		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkSemaphore tempSemaphore;
		MX_VK_CHECK_RESULT(vkCreateSemaphore(mManager->getDevice(), &createInfo, nullptr, &tempSemaphore));
		mSemaphores.push_back(tempSemaphore);
		return tempSemaphore;
	}

	void MxVkSyncObjectPool::destroySemaphore(const VkSemaphore semaphore)
	{
		auto it = std::find(mSemaphores.cbegin(), mSemaphores.cend(), semaphore);
		if (it == mSemaphores.cend())
			throw std::runtime_error("Error : Can not find fence");

		vkDestroyFence(mManager->getDevice(), semaphore, nullptr);
		mSemaphores.erase(it);
	}

	void MxVkSyncObjectPool::destroy()
	{
		for (auto& fence : mFences)
			vkDestroyFence(mManager->getDevice(), fence, nullptr);
		for (auto& semaphore : mSemaphores)
			vkDestroySemaphore(mManager->getDevice(), semaphore, nullptr);
	}

	MxVkSyncObjectPool::~MxVkSyncObjectPool()
	{
		destroy();
	}
}
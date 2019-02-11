#pragma once
#ifndef _MX_VK_SYNC_OBJECT_POOL_H_
#define _MX_VK_SYNC_OBJECT_POOL_H_

#include"MxVkComponent.h"

#include<vector>

namespace Mix
{
	class MxVkSyncObjectPool :public MxVkComponent
	{
	private:
		std::vector<VkFence> mFences;
		std::vector<VkSemaphore> mSemaphores;
	public:
		MxVkSyncObjectPool();
		VkFence createFence(const VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
		void destroyFence(const VkFence fence);
		VkSemaphore createSemaphore();
		void destroySemaphore(const VkSemaphore semaphore);
		void destroy();
		~MxVkSyncObjectPool();
	};
}
#endif // !_MX_VK_SYNC_OBJECT_POOL_H_


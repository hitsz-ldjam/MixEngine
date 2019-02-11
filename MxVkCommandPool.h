#pragma once
#ifndef _MX_VK_COMMAND_H_
#define _MX_VK_COMMAND_H_

#include"MxVkComponent.h"

#include<vector>
#include<list>
#include<initializer_list>
#include<utility>

namespace Mix
{
	class MxVkCommandPool:public MxVkComponent
	{
	private:
		VkQueue mQueue;

		VkCommandPool mCommandPool;
		std::vector<VkCommandBuffer> mCommandBuffers;

		static VkCommandBufferAllocateInfo sTempBufferAllocInfo;
		static VkCommandBufferBeginInfo sTempBufferBeginInfo;

	public:

		MxVkCommandPool();
		bool createCommandPool(VkQueueFlagBits queueType);
		std::vector<VkCommandBuffer> allocCommandBuffers(VkCommandBufferLevel level, uint32_t count);
		void freeCommandBuffers(const std::vector<VkCommandBuffer>& commandBuffers);
		VkCommandBuffer beginTempCommandBuffer();
		void endTempCommandBuffer(VkCommandBuffer commandBuffer);
		void destroy();
		~MxVkCommandPool();
	};

	bool beginCommandBuffer(const VkCommandBuffer commandBuffer, VkCommandBufferUsageFlagBits usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	bool endCommandBuffer(const VkCommandBuffer commandBuffer);
}
#endif // !_MX_VK_COMMAND_H_

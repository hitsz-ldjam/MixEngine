#include"MxVkBuffer.h"

namespace Mix
{
	MxVkBuffer * MxVkBuffer::createBuffer(const MxVkManager* manager, const VkBufferUsageFlags usage,
												  const VkMemoryPropertyFlags memoryProperty,
												  const VkDeviceSize size,
												  const VkSharingMode sharingMode, const void * data)
	{
		MxVkBuffer* buffer = new MxVkBuffer;
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = sharingMode;

		MX_VK_CHECK_RESULT(vkCreateBuffer(manager->getDevice(), &createInfo, nullptr, &buffer->buffer));

		VkMemoryRequirements memRequirements = {};
		vkGetBufferMemoryRequirements(manager->getDevice(), buffer->buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = manager->getMemoryTypeIndex(memRequirements.memoryTypeBits, memoryProperty);
		MX_VK_CHECK_RESULT(vkAllocateMemory(manager->getDevice(), &allocInfo, nullptr, &buffer->memory));

		buffer->manager = manager;
		buffer->alignment = memRequirements.alignment;
		buffer->size = memRequirements.size;
		buffer->usages = usage;
		buffer->memoryProperty = memoryProperty;

		if (data && memoryProperty & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			MX_VK_CHECK_RESULT(buffer->map());
			buffer->copyTo(data, size);

			//if not HOST_COHERENT then need to be flushed
			if ((memoryProperty & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
				buffer->flush();
			buffer->unmap();
		}

		buffer->setupDescriptor();
		MX_VK_CHECK_RESULT(buffer->bind());
		return buffer;
	}

	void MxVkBuffer::copyToDeviceBuffer(const MxVkManager * manager, MxVkCommandPool* commandPool, const MxVkBuffer* dstBuffer, const void * data)
	{
		if (!data)
			throw std::runtime_error("Error : Pointer [ data ] is NULL");

		MxVkBuffer* tempBuffer = createBuffer(manager, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
												  dstBuffer->size, VK_SHARING_MODE_EXCLUSIVE, data);

		VkBufferCopy copyRegion = {};
		copyRegion.size = dstBuffer->size;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;

		VkCommandBuffer commandBuffer = commandPool->beginTempCommandBuffer();
		vkCmdCopyBuffer(commandBuffer, tempBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);
		commandPool->endTempCommandBuffer(commandBuffer);
		delete tempBuffer;
	}

	VkResult MxVkBuffer::map(const VkDeviceSize size, const VkDeviceSize offset)
	{
		return vkMapMemory(manager->getDevice(), memory, offset, size, 0, &mapped);
	}

	void MxVkBuffer::unmap()
	{
		if (mapped)
		{
			vkUnmapMemory(manager->getDevice(), memory);
			mapped = nullptr;
		}
	}

	VkResult MxVkBuffer::bind(const VkDeviceSize offset)
	{
		return vkBindBufferMemory(manager->getDevice(), buffer, memory, offset);
	}

	void MxVkBuffer::setupDescriptor(const VkDeviceSize size, const VkDeviceSize offset)
	{
		descriptor.offset = offset;
		descriptor.buffer = buffer;
		descriptor.range = size;
	}

	void MxVkBuffer::copyTo(const void * data, const VkDeviceSize size)
	{
		assert(data);
		memcpy(mapped, data, static_cast<size_t>(size));
	}

	VkResult MxVkBuffer::flush(const VkDeviceSize size, const VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(manager->getDevice(), 1, &mappedRange);
	}

	inline VkResult MxVkBuffer::invalidate(const VkDeviceSize size, const VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(manager->getDevice(), 1, &mappedRange);
	}

	void MxVkBuffer::destory()
	{
		if (buffer)
		{
			vkDestroyBuffer(manager->getDevice(), buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}
		if (memory)
		{
			vkFreeMemory(manager->getDevice(), memory, nullptr);
			memory = VK_NULL_HANDLE;
		}
	}


}
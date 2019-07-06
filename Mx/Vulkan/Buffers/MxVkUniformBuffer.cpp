#include "MxVkUniformBuffer.h"

namespace Mix {
	namespace Graphics {

		DynamicUniformBuffer::DynamicUniformBuffer(const std::shared_ptr<DeviceAllocator>& _allocator,
												   const uint32_t _sizePerObj, const uint32_t _count) : mCurrCount(0) {
			auto physicalDevice = _allocator->getDevice()->getPhysicalDevice();
			const auto align = physicalDevice->getProperties().limits.minUniformBufferOffsetAlignment;
			const auto maxSize = physicalDevice->getProperties().limits.maxUniformBufferRange;

			const auto actualAlign = Math::Align(_sizePerObj, static_cast<uint32_t>(align));

			const auto maxCount = maxSize / actualAlign;
			const auto actualCount = std::min(maxCount, _count);

			mUniformSize = actualAlign;
			mMaxUniformCount = actualCount;

			mBuffer = std::make_unique<Buffer>(_allocator,
											   vk::BufferUsageFlagBits::eUniformBuffer,
											   vk::MemoryPropertyFlagBits::eHostVisible |
											   vk::MemoryPropertyFlagBits::eHostCoherent,
											   actualCount * actualAlign);
		}

		WriteDescriptorSet DynamicUniformBuffer::getWriteDescriptorSet(const vk::DescriptorSet& _set,
																	   const uint32_t _binding,
																	   std::optional<Utils::OffsetSize<uint32_t>>
																	   _offsetSize) const {
			vk::DescriptorBufferInfo bufferInfo = mBuffer->descriptorInfo();
			bufferInfo.range = mUniformSize;

			if (_offsetSize) {
				bufferInfo.offset = _offsetSize.value().offset;
				bufferInfo.range = _offsetSize.value().size;
			}

			vk::WriteDescriptorSet write;
			write.dstSet = _set;     //descriptor which will be write in
			write.dstBinding = _binding; //destination binding
			write.dstArrayElement = 0;
			write.descriptorType = vk::DescriptorType::eUniformBufferDynamic;
			//the type of the descriptor that will be wirte in
			write.descriptorCount = 1;           //descriptor count
			write.pBufferInfo = &bufferInfo; //descriptorBufferInfo
			write.pImageInfo = nullptr;
			write.pTexelBufferView = nullptr;

			return WriteDescriptorSet(write, bufferInfo);
		}
	}
}
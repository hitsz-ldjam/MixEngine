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

		WriteDescriptorSet DynamicUniformBuffer::getWriteDescriptorSet(const uint32_t _binding) const {
			return mBuffer->getWriteDescriptor(_binding, vk::DescriptorType::eUniformBufferDynamic, OffsetSize64{ 0,mUniformSize });
		}
	}
}
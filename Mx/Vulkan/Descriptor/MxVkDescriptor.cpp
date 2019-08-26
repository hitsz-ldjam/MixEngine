#include "MxVkDescriptor.h"

namespace Mix {

	Vulkan::WriteDescriptorSet::WriteDescriptorSet(const vk::WriteDescriptorSet& _writeDescriptorSet,
												   const vk::DescriptorImageInfo& _imageInfo) :
		mWriteDescriptorSet(_writeDescriptorSet),
		mImageInfo(_imageInfo),
		mBufferInfo(nullptr) {
		checkInfoPtr();
	}

	Vulkan::WriteDescriptorSet::WriteDescriptorSet(const vk::WriteDescriptorSet& _writeDescriptorSet,
												   const vk::DescriptorBufferInfo& _bufferInfo) :
		mWriteDescriptorSet(_writeDescriptorSet),
		mImageInfo(nullptr),
		mBufferInfo(_bufferInfo) {
		mWriteDescriptorSet.pBufferInfo = &mBufferInfo.value();
		checkInfoPtr();
	}

	Vulkan::WriteDescriptorSet::WriteDescriptorSet(const WriteDescriptorSet& _other)
		:mWriteDescriptorSet(_other.mWriteDescriptorSet),
		mImageInfo(_other.mImageInfo),
		mBufferInfo(_other.mBufferInfo) {
		checkInfoPtr();
	}

	Vulkan::WriteDescriptorSet::WriteDescriptorSet(WriteDescriptorSet&& _other) noexcept :
		mWriteDescriptorSet(_other.mWriteDescriptorSet),
		mImageInfo(_other.mImageInfo),
		mBufferInfo(_other.mBufferInfo) {
		checkInfoPtr();
	}

	Vulkan::WriteDescriptorSet& Vulkan::WriteDescriptorSet::operator=(WriteDescriptorSet _other) {
		swap(_other);
		return *this;
	}

	void Vulkan::WriteDescriptorSet::swap(WriteDescriptorSet& _other) noexcept {
		std::swap(mWriteDescriptorSet, _other.mWriteDescriptorSet);
		std::swap(mImageInfo, _other.mImageInfo);
		std::swap(mBufferInfo, _other.mBufferInfo);
		checkInfoPtr();
	}

	void Vulkan::WriteDescriptorSet::checkInfoPtr() noexcept {
		mWriteDescriptorSet.pImageInfo = mImageInfo.has_value() ? &mImageInfo.value() : nullptr;
		mWriteDescriptorSet.pBufferInfo = mBufferInfo.has_value() ? &mBufferInfo.value() : nullptr;
	}
}

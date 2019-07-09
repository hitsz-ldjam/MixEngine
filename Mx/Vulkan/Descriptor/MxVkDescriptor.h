#pragma once
#ifndef MX_VK_DESCRIPTOR_H_
#define MX_VK_DESCRIPTOR_H_

#include <vulkan/vulkan.hpp>
#include <optional>
#include "../../Utils/MxOffsetSize.hpp"

namespace Mix {
	namespace Graphics {
		class  WriteDescriptorSet {
		public:
			WriteDescriptorSet(const vk::WriteDescriptorSet &_writeDescriptorSet, const vk::DescriptorImageInfo &_imageInfo) :
				mWriteDescriptorSet(_writeDescriptorSet),
				mImageInfo(_imageInfo),
				mBufferInfo(nullptr) {
				mWriteDescriptorSet.pImageInfo = &mImageInfo.value();
			}

			WriteDescriptorSet(const vk::WriteDescriptorSet &_writeDescriptorSet, const vk::DescriptorBufferInfo &_bufferInfo) :
				mWriteDescriptorSet(_writeDescriptorSet),
				mImageInfo(nullptr),
				mBufferInfo(_bufferInfo) {
				mWriteDescriptorSet.pBufferInfo = &mBufferInfo.value();
			}

			const vk::WriteDescriptorSet &get() const { return mWriteDescriptorSet; }

		private:
			vk::WriteDescriptorSet mWriteDescriptorSet;
			std::optional<vk::DescriptorImageInfo> mImageInfo;
			std::optional<vk::DescriptorBufferInfo> mBufferInfo;
		};

		class Descriptor {
		public:
			virtual WriteDescriptorSet getWriteDescriptor(const uint32_t &_binding, const vk::DescriptorType &_descriptorType, const std::optional<OffsetSize64>& _offsetSize = std::nullopt) const = 0;

			Descriptor() = default;

			virtual ~Descriptor() = default;
		};
	}
}

#endif

#pragma once
#ifndef MX_VK_DESCRIPTOR_H_
#define MX_VK_DESCRIPTOR_H_

#include <vulkan/vulkan.hpp>
#include <optional>
#include "../../Utils/MxOffsetSize.hpp"

namespace Mix {
	namespace Vulkan {
		class  WriteDescriptorSet {
		public:
			WriteDescriptorSet() = default;

			WriteDescriptorSet(const vk::WriteDescriptorSet& _writeDescriptorSet,
			                   const vk::DescriptorImageInfo& _imageInfo);

			WriteDescriptorSet(const vk::WriteDescriptorSet& _writeDescriptorSet,
			                   const vk::DescriptorBufferInfo& _bufferInfo);


			WriteDescriptorSet(const WriteDescriptorSet& _other);

			WriteDescriptorSet(WriteDescriptorSet&& _other) noexcept;

			WriteDescriptorSet& operator=(WriteDescriptorSet _other);

			void swap(WriteDescriptorSet& _other) noexcept;

			void setDstSet(const vk::DescriptorSet& _set) { mWriteDescriptorSet.dstSet = _set; }

			const vk::WriteDescriptorSet &get() const { return mWriteDescriptorSet; }

			vk::WriteDescriptorSet& get() { return mWriteDescriptorSet; }

		private:
			void checkInfoPtr() noexcept;

			vk::WriteDescriptorSet mWriteDescriptorSet;
			std::optional<vk::DescriptorImageInfo> mImageInfo;
			std::optional<vk::DescriptorBufferInfo> mBufferInfo;
		};

		class Descriptor {
		public:
			virtual WriteDescriptorSet getWriteDescriptor(uint32_t _binding, vk::DescriptorType _descriptorType, const std::optional<OffsetSize64>& _offsetSize = std::nullopt) const = 0;

			Descriptor() = default;

			virtual ~Descriptor() = default;
		};
	}
}

#endif

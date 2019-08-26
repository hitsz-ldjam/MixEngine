#pragma once
#ifndef MX_VK_BUFFER_H_
#define MX_VK_BUFFER_H_

#include <memory>
#include "../Memory/MxVkAllocator.h"
#include "../../Utils/MxGeneralBase.hpp"
#include "../Descriptor/MxVkDescriptor.h"

// #define BUFFER_MANAGER_ENABLE

namespace Mix {
	namespace Vulkan {
		class Device;

		class Buffer : public GeneralBase::NoCopyBase, public Descriptor {
		public:
			Buffer(const std::shared_ptr<DeviceAllocator>& _allocator,
				   const vk::BufferUsageFlags& _usage,
				   const vk::MemoryPropertyFlags& _memoryProperty,
				   const vk::DeviceSize _size,
				   const void* _data = nullptr,
				   const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive);

			Buffer(Buffer&& _other) noexcept { swap(_other); }

			Buffer& operator=(Buffer&& _other) noexcept { swap(_other); return *this; }

			void swap(Buffer& _other) noexcept;

			~Buffer();

			const vk::Buffer& get() const { return mBuffer; }

			const vk::DeviceSize& size() const { return mSize; }

			const vk::DeviceSize& alignment() const { return mAlignment; }

			WriteDescriptorSet getWriteDescriptor(uint32_t _binding, vk::DescriptorType _descriptorType, const std::optional<OffsetSize64>& _offsetSize = std::nullopt) const override;

			void setData(const void* _data, const vk::DeviceSize& _size);

			void setData(const void* _data, const OffsetSize64& _offsetSize);

			void setData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const;

			void setData(const std::vector<char>& _data, const vk::DeviceSize& _offset = 0);

			std::vector<char> getData(const OffsetSize64& _offsetSize);

			std::vector<char> getData();

			void flush(const vk::DeviceSize _size = VK_WHOLE_SIZE, const vk::DeviceSize _offset = 0) const;

			void invalidate(const vk::DeviceSize _size = VK_WHOLE_SIZE, const vk::DeviceSize _offset = 0) const;

			void* rawPtr() const { return mMemory.ptr; }

		protected:
			vk::Buffer mBuffer;
			vk::DeviceSize mSize{};
			vk::DeviceSize mAlignment{};
			MemoryBlock mMemory;
			vk::BufferUsageFlags mUsages;
			vk::MemoryPropertyFlags mMemoryProperty;

			std::shared_ptr<DeviceAllocator> mAllocator;

		};
	}
}
#endif // !MX_VK_BUFFER_H_

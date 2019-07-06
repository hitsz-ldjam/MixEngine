#pragma once
#ifndef MX_VK_ALLOCATOR_H_
#define MX_VK_ALLOCATOR_H_

#include "../Device/MxVkDevice.h"
#include "../../Math/MxMath.h"

namespace Mix {
	namespace Graphics {
		struct MemoryBlock {
			vk::DeviceMemory memory = nullptr;
			vk::DeviceSize offset = 0;
			vk::DeviceSize size = 0;

			bool free = false;
			void* ptr = nullptr;

			bool operator==(const MemoryBlock& _block) const {
				return (memory == _block.memory &&
						offset == _block.offset &&
						size == _block.size &&
						free == _block.free &&
						ptr == _block.ptr);
			}
		};

		class Chunk :public GeneralBase::NoCopyBase {
		public:
			Chunk(const std::shared_ptr<Device>& _device,
				  const vk::DeviceSize& _size,
				  uint32_t _memoryTypeIndex);

			Chunk(Chunk&& _chunk) noexcept;

			Chunk& operator=(Chunk&& _chunk) noexcept;

			~Chunk();

			bool allocate(const vk::DeviceSize& _size, const vk::DeviceSize& _alignment, MemoryBlock& _block);

			bool isIn(const MemoryBlock& _block) const {
				return mMem == _block.memory;
			}
			void deallocate(const MemoryBlock& _block);

			uint32_t memoryTypeIndex() const {
				return mMemTypeIndex;
			}

		private:
			std::shared_ptr<Device> mDevice;
			vk::DeviceMemory mMem = nullptr;
			vk::DeviceSize mSize;

			uint32_t mMemTypeIndex;
			std::vector<MemoryBlock> mBlocks;
			void* mPtr = nullptr;

		};

		class ChunkFactory {
			static const vk::DeviceSize MinChunkSize = 8 * 1024 * 1024;
		public:
			ChunkFactory() = default;

			explicit ChunkFactory(const std::shared_ptr<Device>& _device) :mDevice(_device) {}

			void setMinChunkSize(const vk::DeviceSize _size) {
				assert(Math::IsPowerOf2(_size));
				mMinChunkSize = _size;
			}

			std::unique_ptr<Chunk> getChunk(vk::DeviceSize _size, uint32_t _memTypeIndex);

		private:
			std::shared_ptr<Device> mDevice;
			vk::DeviceSize mMinChunkSize = MinChunkSize;
		};

		class AbstractAllocator : public GeneralBase::NoCopyBase {
		public:
			virtual MemoryBlock allocate(const vk::DeviceSize& _size, const vk::DeviceSize& _alignment, uint32_t _memoryTypeIndex) = 0;
			virtual void deallocate(MemoryBlock &_block) = 0;

			virtual ~AbstractAllocator() = default;
		};

		class DeviceAllocator :public AbstractAllocator {
		public:
			explicit DeviceAllocator(const std::shared_ptr<Device>& _device)
				: mDevice(_device), mChunkFactory(_device) {
			}

			DeviceAllocator(DeviceAllocator&& _other) noexcept { swap(_other); }

			DeviceAllocator& operator=(DeviceAllocator&& _other) noexcept { swap(_other); return *this; }

			void swap(DeviceAllocator& _other) noexcept;

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			MemoryBlock allocate(const vk::DeviceSize& _size,
								 const vk::DeviceSize& _alignment,
								 uint32_t _memoryTypeIndex) override;

			MemoryBlock allocate(const vk::Image& _image,
								 const vk::MemoryPropertyFlags & _properties,
								 vk::MemoryRequirements* _memReq = nullptr);

			MemoryBlock allocate(const vk::Buffer& _buffer,
								 const vk::MemoryPropertyFlags& _properties,
								 vk::MemoryRequirements* _memReq = nullptr);

			void deallocate(MemoryBlock& _block) override;

		private:
			std::shared_ptr<Device> mDevice;
			ChunkFactory mChunkFactory;
			std::vector<std::unique_ptr<Chunk>> mChunks;
		};
	}
}

#endif // !MX_VK_ALLOCATOR_H_
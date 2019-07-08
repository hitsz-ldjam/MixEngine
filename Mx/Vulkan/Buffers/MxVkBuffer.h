#pragma once
#ifndef MX_VK_BUFFER_H_
#define MX_VK_BUFFER_H_

#include <memory>
#include "../Core/MxVkCore.h"
#include "../Memory/MxVkAllocator.h"
#include "../../Utils/MxGeneralBase.hpp"

// #define BUFFER_MANAGER_ENABLE

namespace Mix {
	namespace Graphics {
		class Device;

		class Buffer : public GeneralBase::NoCopyBase {
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

			const vk::DescriptorBufferInfo& descriptorInfo() const { return mDescriptor; };

			void uploadData(const void* _data, const vk::DeviceSize& _size) const;

			void uploadData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const;

			void flush(const vk::DeviceSize _size = VK_WHOLE_SIZE, const vk::DeviceSize _offset = 0) const;

			void invalidate(const vk::DeviceSize _size = VK_WHOLE_SIZE, const vk::DeviceSize _offset = 0) const;

		private:
			vk::Buffer mBuffer;
			vk::DeviceSize mSize{};
			vk::DeviceSize mAlignment{};
			MemoryBlock mMemory;
			vk::DescriptorBufferInfo mDescriptor;
			vk::BufferUsageFlags mUsages;
			vk::MemoryPropertyFlags mMemoryProperty;

			std::shared_ptr<DeviceAllocator> mAllocator;

			void setupDescriptor(const vk::DeviceSize _size = VK_WHOLE_SIZE, const vk::DeviceSize _offset = 0);

		};

#ifdef BUFFER_MANAGER_ENABLE

		// buffer manager
		// BufferManager will create a large buffer for one chunk and sub-Allocate from it
		namespace Test {
			struct BufferBlock {
				vk::Buffer buffer = nullptr;
				vk::DeviceSize offset = 0;
				vk::DeviceSize size = 0;
				bool free = false;
				MemoryBlock memory;
			};

			struct Buffer {
				BufferBlock bufferBlock;
				vk::DeviceSize alignment;

				vk::BufferUsageFlags usages;
				vk::MemoryPropertyFlags memoryProperty;

				Buffer(const BufferBlock& _block, const vk::DeviceSize& _alignment, const vk::BufferUsageFlags& _usages, const vk::MemoryPropertyFlags& memProperty) :
					bufferBlock(_block), alignment(_alignment), usages(_usages), memoryProperty(memProperty) {
				}

				void copyTo(const void * _data, const vk::DeviceSize _size) const;
			};

			class BufferChunk {
			public:
				BufferChunk(std::shared_ptr<Core> _core,
							std::shared_ptr<DeviceAllocator> _allocator,
							const vk::BufferUsageFlags _usage,
							const vk::MemoryPropertyFlags _memoryProperty,
							const vk::DeviceSize _size,
							const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive);

				BufferChunk(const Chunk& _chunk) = delete;
				BufferChunk(BufferChunk&& _chunk) {
					*this = std::move(_chunk);
				}

				BufferChunk& operator=(const Chunk& chunk) = delete;

				BufferChunk& operator=(BufferChunk&& _chunk);

				~BufferChunk();

				bool createBuffer(vk::DeviceSize _size, BufferBlock& _block);

				void destroyBuffer(const BufferBlock& _block);

				bool isIn(const BufferBlock& _buffer) const {
					return mBuffer == _buffer.buffer;
				}

			private:
				std::shared_ptr<Core> mCore;
				std::shared_ptr<DeviceAllocator> mAllocator;
				vk::Buffer mBuffer;
				vk::DeviceSize mSize;
				vk::MemoryRequirements mMemReq;
				vk::BufferUsageFlags mBufferUsage;

				MemoryBlock mMemBlock;
				vk::MemoryPropertyFlags mMemProperty;

				std::vector<BufferBlock> mBufferBlocks;

				// todo 
				uint32_t mOperationCount = 0;
				static uint32_t MaxOperationCount;

				void sortOut() {}
			};

			class BufferChunkFactory :public GraphicsComponent {
			public:
				void setMinChunkSize(const vk::DeviceSize _size) {
					assert(Utils::IsPowerOf2(_size));
					mMinChunkSize = _size;
				}

				std::unique_ptr<BufferChunk> getChunk(const vk::BufferUsageFlags& _usage,
													  const vk::MemoryPropertyFlags& _memoryProperty,
													  vk::DeviceSize _size,
													  const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive);

			private:
				std::shared_ptr<DeviceAllocator> mAllocator;
				vk::DeviceSize mMinChunkSize = 8 * 1024 * 1024;
			};

			class AbstractBufferManager :public GraphicsComponent {
			public:
				AbstractBufferManager() = default;
				AbstractBufferManager(const AbstractBufferManager&) = delete;
				AbstractBufferManager(AbstractBufferManager&& _manager) {
					*this = std::move(_manager);
				}
				virtual ~AbstractBufferManager() = 0 {};

				AbstractBufferManager& operator=(const AbstractBufferManager&) = delete;
				AbstractBufferManager& operator=(AbstractBufferManager&& _manager) {
					mCore = _manager.mCore;
					mChunks = std::move(_manager.mChunks);
					return *this;
				}

			private:
				BufferChunkFactory mChunkFactory;
				std::vector<BufferChunk> mChunks;
				std::shared_ptr<DeviceAllocator> mAllocator;
			};


			class UniformBufferManager :public AbstractBufferManager {

			};

		}

#endif // BUFFER_MANAGER_ENABLE
	}
}
#endif // !MX_VK_BUFFER_H_

#include "MxVkBuffer.h"
#include "../Device/MxVkDevice.h"

namespace Mix {
	namespace Graphics {
		Buffer::Buffer(const std::shared_ptr<DeviceAllocator>& _allocator,
					   const vk::BufferUsageFlags& _usage,
					   const vk::MemoryPropertyFlags& _memoryProperty,
					   const vk::DeviceSize _size,
					   const void* _data,
					   const vk::SharingMode _sharingMode)
			: mAllocator(_allocator) {

			vk::BufferCreateInfo createInfo;
			createInfo.size = _size;
			createInfo.usage = _usage;
			createInfo.sharingMode = _sharingMode;

			mBuffer = _allocator->getDevice()->get().createBuffer(createInfo);
			vk::MemoryRequirements memRequirements;
			mMemory = _allocator->allocate(mBuffer, _memoryProperty, &memRequirements);

			mAlignment = memRequirements.alignment;
			mSize = _size;
			mUsages = _usage;
			mMemoryProperty = _memoryProperty;

			// copy data to the buffer
			if (_data) {
				if (_memoryProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
					uploadData(_data, _size);

					//if not HOST_COHERENT then need to be flushed
					if (!(_memoryProperty & vk::MemoryPropertyFlagBits::eHostCoherent))
						flush();
				}
			}
		}

		void Buffer::swap(Buffer& _other) noexcept {
			using std::swap;
			swap(mBuffer, _other.mBuffer);
			swap(mSize, _other.mSize);
			swap(mAlignment, _other.mAlignment);
			swap(mMemory, _other.mMemory);
			swap(mUsages, _other.mUsages);
			swap(mMemoryProperty, _other.mMemoryProperty);
			swap(mAllocator, _other.mAllocator);
		}

		Buffer::~Buffer() {
			if (mBuffer) {
				mAllocator->getDevice()->get().destroyBuffer(mBuffer);
				mAllocator->deallocate(mMemory);
			}
		}

		WriteDescriptorSet Buffer::getWriteDescriptor(const uint32_t& _binding, const vk::DescriptorType& _descriptorType, const std::optional<OffsetSize64>& _offsetSize) const {
			vk::DescriptorBufferInfo bufferInfo{ mBuffer,0,mSize };

			if (_offsetSize) {
				bufferInfo.offset = _offsetSize.value().offset;
				bufferInfo.range = _offsetSize.value().size;
			}

			vk::WriteDescriptorSet write{ nullptr,_binding,0,1,_descriptorType };
			return WriteDescriptorSet(write, bufferInfo);
		}

		void Buffer::uploadData(const void * _data, const vk::DeviceSize& _size) const {
			assert(_data);
			memcpy(mMemory.ptr, _data, static_cast<size_t>(_size));
		}

		void Buffer::uploadData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const {
			assert(_data && _dstOffset + _size <= mSize);
			memcpy(static_cast<char*>(mMemory.ptr) + _dstOffset, _data, static_cast<size_t>(_size));
		}

		void Buffer::flush(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
			vk::MappedMemoryRange mappedRange;
			mappedRange.memory = mMemory.memory;
			mappedRange.offset = _offset;
			mappedRange.size = _size;
			mAllocator->getDevice()->get().flushMappedMemoryRanges(mappedRange);
		}

		void Buffer::invalidate(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
			vk::MappedMemoryRange mappedRange = {};
			mappedRange.memory = mMemory.memory;
			mappedRange.offset = _offset;
			mappedRange.size = _size;
			mAllocator->getDevice()->get().invalidateMappedMemoryRanges(mappedRange);
		}

#ifdef BUFFER_MANAGER_ENABLE

		namespace Test {
			void Buffer::copyTo(const void * _data, const vk::DeviceSize _size) const {
				assert(_data);
				memcpy(bufferBlock.memory.ptr, _data, static_cast<size_t>(_size));
			}

			BufferChunk::BufferChunk(std::shared_ptr<Core> _core,
									 std::shared_ptr<DeviceAllocator> _allocator,
									 const vk::BufferUsageFlags _usage,
									 const vk::MemoryPropertyFlags _memoryProperty,
									 const vk::DeviceSize _size,
									 const vk::SharingMode _sharingMode) :
				mCore(_core),
				mAllocator(_allocator) {
				vk::BufferCreateInfo createInfo;
				createInfo.usage = _usage;
				createInfo.size = _size;
				createInfo.sharingMode = _sharingMode;

				mBuffer = mCore->getDevice().createBuffer(createInfo);
				mMemBlock = mAllocator->allocate(mBuffer, _memoryProperty, &mMemReq);
				mBufferUsage = _usage;
				mMemProperty = _memoryProperty;

				BufferBlock block;
				block.buffer = mBuffer;
				block.offset = 0;
				block.size = _size;
				block.memory = mMemBlock;

				mBufferBlocks.push_back(block);
			}

			BufferChunk & BufferChunk::operator=(BufferChunk && _chunk) {
				mCore = _chunk.mCore;
				mAllocator = _chunk.mAllocator;
				mMemBlock = _chunk.mMemBlock;

				mBufferBlocks = std::move(_chunk.mBufferBlocks);

				_chunk.mBuffer = nullptr;
				return *this;
			}

			BufferChunk::~BufferChunk() {
				if (mBuffer) {
					mCore->getDevice().destroyBuffer(mBuffer);
					mAllocator->deallocate(mMemBlock);
				}
			}

			bool BufferChunk::createBuffer(vk::DeviceSize _size, BufferBlock & _block) {
				if (_size > mSize)
					return false;

				for (size_t i = 0; i < mBufferBlocks.size(); ++i) {
					// if block is unused
					if (mBufferBlocks[i].free) {
						// compute new size
						const vk::DeviceSize newSize = Utils::Align(_size, mMemReq.alignment);

						// if block is suitable
						if (mBufferBlocks[i].size >= newSize) {
							// if block is perfect match
							if (mBufferBlocks[i].size == newSize) {
								mBufferBlocks[i].free = false;
								_block = mBufferBlocks[i];
								return true;
							}

							// if there are space left
							BufferBlock nextBlock;
							nextBlock.free = true;
							nextBlock.buffer = mBufferBlocks[i].buffer;
							nextBlock.offset = mBufferBlocks[i].offset + newSize;
							nextBlock.size = mBufferBlocks[i].size - newSize;

							nextBlock.memory = mBufferBlocks[i].memory;
							nextBlock.memory.offset += newSize;
							nextBlock.memory.size = mBufferBlocks[i].size - newSize;
							if (mBufferBlocks[i].memory.ptr)
								nextBlock.memory.ptr = static_cast<char*>(nextBlock.memory.ptr) + newSize;

							mBufferBlocks.emplace_back(nextBlock);

							mBufferBlocks[i].size = _size;
							mBufferBlocks[i].free = false;

							_block = mBufferBlocks[i];
							return true;
						}
					}
				}
				return false;
			}

			void BufferChunk::destroyBuffer(const BufferBlock& _block) {
				const auto it = std::find(mBufferBlocks.begin(), mBufferBlocks.end(), _block);
				assert(it != mBufferBlocks.end());

				it->free = true;

				if ((++mOperationCount) >= MaxOperationCount)
					sortOut();
			}

			std::unique_ptr<BufferChunk> BufferChunkFactory::getChunk(const vk::BufferUsageFlags& _usage,
																	  const vk::MemoryPropertyFlags& _memoryProperty,
																	  vk::DeviceSize _size,
																	  const vk::SharingMode _sharingMode) {
				_size = (mMinChunkSize < _size) ? Utils::NextPowerOf2(_size) : mMinChunkSize;

				return std::make_unique<BufferChunk>(mCore,
													 mAllocator,
													 _usage,
													 _memoryProperty,
													 _size,
													 _sharingMode);
			}
		}

#endif // BUFFER_MANAGER_ENABLE
	}
}

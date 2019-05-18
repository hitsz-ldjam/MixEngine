#include "MxVkBuffer.h"

namespace Mix {
    namespace Graphics {
        std::shared_ptr<Buffer> Buffer::CreateBuffer(std::shared_ptr<Core>            _core,
                                                     std::shared_ptr<DeviceAllocator> _allocator,
                                                     const vk::BufferUsageFlags&      _usage,
                                                     const vk::MemoryPropertyFlags&   _memoryProperty,
                                                     const vk::DeviceSize             _size,
                                                     const vk::SharingMode            _sharingMode,
                                                     const void*                      _data) {
            auto buffer = std::make_shared<Buffer>();
            buffer->init(_core, _allocator);

            vk::BufferCreateInfo createInfo;
            createInfo.size = _size;
            createInfo.usage = _usage;
            createInfo.sharingMode = _sharingMode;

            buffer->buffer = _core->getDevice().createBuffer(createInfo);

            vk::MemoryRequirements memRequirements;
            buffer->memory = _allocator->allocate(buffer->buffer, _memoryProperty, &memRequirements);

            buffer->alignment = memRequirements.alignment;
            buffer->size = _size;
            buffer->usages = _usage;
            buffer->memoryProperty = _memoryProperty;

            buffer->setupDescriptor(_size, 0);

            // copy data to the buffer
            if (_data) {
                if (_memoryProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
                    buffer->copyTo(_data, _size);

                    //if not HOST_COHERENT then need to be flushed
                    if (!(_memoryProperty & vk::MemoryPropertyFlagBits::eHostCoherent))
                        buffer->flush();
                }
            }

            return buffer;
        }

        void Buffer::init(std::shared_ptr<Core> _core, std::shared_ptr<DeviceAllocator> _allocator) {
            mCore = _core;
            mAllocator = _allocator;
        }

        void Buffer::setupDescriptor(const vk::DeviceSize _size, const vk::DeviceSize _offset) {
            descriptor.offset = _offset;
            descriptor.buffer = buffer;
            descriptor.range = _size;
        }

        void Buffer::copyTo(const void * _data, const vk::DeviceSize _size) const {
            assert(_data);
            memcpy(memory.ptr, _data, static_cast<size_t>(_size));
        }

        void Buffer::flush(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
            vk::MappedMemoryRange mappedRange;
            mappedRange.memory = memory.memory;
            mappedRange.offset = _offset;
            mappedRange.size = _size;
            mCore->getDevice().flushMappedMemoryRanges(mappedRange);
        }

        void Buffer::invalidate(const vk::DeviceSize _size, const vk::DeviceSize _offset) const {
            vk::MappedMemoryRange mappedRange = {};
            mappedRange.memory = memory.memory;
            mappedRange.offset = _offset;
            mappedRange.size = _size;
            mCore->getDevice().invalidateMappedMemoryRanges(mappedRange);
        }

        void Buffer::destory() {
            if (!mCore)
                return;

            if (buffer) {
                mCore->getDevice().destroyBuffer(buffer);
                buffer = nullptr;

                mAllocator->deallocate(memory);
                memory = MemoryBlock();
            }

            mCore = nullptr;
            mAllocator = nullptr;
        }

#ifdef BUFFER_MANAGER_ENABLE

        namespace Test {
            void Buffer::copyTo(const void * data, const vk::DeviceSize size) {
                assert(data);
                memcpy(bufferBlock.memory.ptr, data, static_cast<size_t>(size));
            }

            BufferChunk::BufferChunk(std::shared_ptr<Core> core,
                                     std::shared_ptr<DeviceAllocator> allocator,
                                     const vk::BufferUsageFlags usage,
                                     const vk::MemoryPropertyFlags memoryProperty,
                                     const vk::DeviceSize size,
                                     const vk::SharingMode sharingMode) :
                mCore(core),
                mAllocator(allocator) {
                vk::BufferCreateInfo createInfo;
                createInfo.usage = usage;
                createInfo.size = size;
                createInfo.sharingMode = sharingMode;

                mBuffer = mCore->device().createBuffer(createInfo);
                mMemBlock = mAllocator->Allocate(mBuffer, memoryProperty, &mMemReq);
                mBufferUsage = usage;
                mMemProperty = memoryProperty;

                BufferBlock block;
                block.buffer = mBuffer;
                block.offset = 0;
                block.size = size;
                block.memory = mMemBlock;

                mBufferBlocks.push_back(block);
            }

            BufferChunk & BufferChunk::operator=(BufferChunk && chunk) {
                mCore = chunk.mCore;
                mAllocator = chunk.mAllocator;
                vk::Buffer mBuffer;
                mMemBlock = chunk.mMemBlock;

                mBufferBlocks = std::move(chunk.mBufferBlocks);

                chunk.mBuffer = nullptr;
                return *this;
            }

            BufferChunk::~BufferChunk() {
                if (mBuffer) {
                    mCore->device().destroyBuffer(mBuffer);
                    mAllocator->deallocate(mMemBlock);
                }
            }

            bool BufferChunk::createBuffer(vk::DeviceSize size, BufferBlock & block) {
                if (size > mSize)
                    return false;

                for (size_t i = 0; i < mBufferBlocks.size(); ++i) {
                    // if block is unused
                    if (mBufferBlocks[i].free) {
                        // compute new size
                        vk::DeviceSize newSize = Utils::align(size, mMemReq.alignment);

                        // if block is suitable
                        if (mBufferBlocks[i].size >= newSize) {
                            // if block is perfect match
                            if (mBufferBlocks[i].size == newSize) {
                                mBufferBlocks[i].free = false;
                                block = mBufferBlocks[i];
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

                            mBufferBlocks[i].size = size;
                            mBufferBlocks[i].free = false;

                            block = mBufferBlocks[i];
                            return true;
                        }
                    }
                }
                return false;
            }

            void BufferChunk::destroyBuffer(const BufferBlock& block) {
                auto it = std::find(mBufferBlocks.begin(), mBufferBlocks.end(), block);
                assert(it != mBufferBlocks.end());

                it->free = true;

                if ((++mOperationCount) >= MaxOperationCount)
                    sortOut();
            }

            std::unique_ptr<BufferChunk> BufferChunkFactory::getChunk(const vk::BufferUsageFlags& usage,
                                                                      const vk::MemoryPropertyFlags& memoryProperty,
                                                                      vk::DeviceSize size,
                                                                      const vk::SharingMode sharingMode) {
                size = (mMinChunkSize < size) ? Utils::nextPowerOf2(size) : mMinChunkSize;

                return std::make_unique<BufferChunk>(mCore,
                                                     mAllocator,
                                                     usage,
                                                     memoryProperty,
                                                     size,
                                                     sharingMode);
            }
        }

#endif // BUFFER_MANAGER_ENABLE
    }
}

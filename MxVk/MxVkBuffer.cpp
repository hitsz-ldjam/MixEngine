#include "MxVkBuffer.h"

namespace Mix {
    namespace Graphics {
        Buffer * Buffer::createBuffer(std::shared_ptr<Core> core,
                                      std::shared_ptr<DeviceAllocator> allocator,
                                      const vk::BufferUsageFlags usage,
                                      const vk::MemoryPropertyFlags memoryProperty,
                                      const vk::DeviceSize size,
                                      const void * data,
                                      const vk::CommandBuffer& cmd,
                                      const vk::SharingMode sharingMode) {
            Buffer* buffer = new Buffer;
            buffer->init(core, allocator);

            vk::BufferCreateInfo createInfo;
            createInfo.size = size;
            createInfo.usage = usage;
            createInfo.sharingMode = sharingMode;

            buffer->buffer = core->device().createBuffer(createInfo);

            vk::MemoryRequirements memRequirements;
            buffer->memory = allocator->allocate(buffer->buffer, memoryProperty, &memRequirements);

            buffer->alignment = memRequirements.alignment;
            buffer->size = size;
            buffer->usages = usage;
            buffer->memoryProperty = memoryProperty;

            if (data && memoryProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
                buffer->copyTo(data, size);

                //if not HOST_COHERENT then need to be flushed
                if (!(memoryProperty & vk::MemoryPropertyFlagBits::eHostCoherent))
                    buffer->flush();
            }

            buffer->setupDescriptor(buffer->size, 0);
            return buffer;
        }

        void Buffer::copyToDeviceBuffer(std::shared_ptr<Core> core,
                                        std::shared_ptr<DeviceAllocator> allocator,
                                        const vk::CommandBuffer& cmd,
                                        const Buffer* dstBuffer,
                                        const void* data) {
            if (!data)
                throw std::runtime_error("Error : Pointer [ data ] is NULL");

            Buffer* tempBuffer = createBuffer(core,
                                              allocator,
                                              vk::BufferUsageFlagBits::eTransferSrc,
                                              vk::MemoryPropertyFlagBits::eHostVisible |
                                              vk::MemoryPropertyFlagBits::eHostCoherent,
                                              dstBuffer->size,
                                              data,
                                              cmd,
                                              vk::SharingMode::eExclusive);

            vk::BufferCopy copyRegion = {};
            copyRegion.size = dstBuffer->size;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;

            cmd.copyBuffer(tempBuffer->buffer, dstBuffer->buffer, copyRegion);
            delete tempBuffer;
        }

        void Buffer::copyToDeviceBuffer(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator, const vk::CommandBuffer & cmd, const std::unique_ptr<Buffer>& dstBuffer, const void * data) {
            copyToDeviceBuffer(core,
                               allocator,
                               cmd,
                               dstBuffer.get(),
                               data);
        }

        void Buffer::init(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator) {
            mCore = core;
            mAllocator = allocator;
        }

        void Buffer::setupDescriptor(const vk::DeviceSize size, const vk::DeviceSize offset) {
            descriptor.offset = offset;
            descriptor.buffer = buffer;
            descriptor.range = size;
        }

        void Buffer::copyTo(const void * data, const vk::DeviceSize size) {
            assert(data);
            memcpy(memory.ptr, data, static_cast<size_t>(size));
        }

        void Buffer::flush(const vk::DeviceSize size, const vk::DeviceSize offset) {
            vk::MappedMemoryRange mappedRange;
            mappedRange.memory = memory.memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            mCore->device().flushMappedMemoryRanges(mappedRange);
        }

        void Buffer::invalidate(const vk::DeviceSize size, const vk::DeviceSize offset) {
            vk::MappedMemoryRange mappedRange = {};
            mappedRange.memory = memory.memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            mCore->device().invalidateMappedMemoryRanges(mappedRange);
        }

        void Buffer::destory() {
            if (!mCore)
                return;

            if (buffer) {
                mCore->device().destroyBuffer(buffer);
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
                mMemBlock = mAllocator->allocate(mBuffer, memoryProperty, &mMemReq);
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

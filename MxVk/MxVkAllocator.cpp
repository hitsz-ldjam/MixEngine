#include "MxVkAllocator.h"

namespace Mix {
    namespace Graphics {
        uint32_t Chunk::MaxOperationCount = 10;

        Chunk::Chunk(std::shared_ptr<Core> core, vk::DeviceSize size, uint32_t memoryTypeIndex) :
            mCore(core),
            mSize(size),
            mMemTypeIndex(memoryTypeIndex) {
            vk::MemoryAllocateInfo allocInfo(size, memoryTypeIndex);

            MemoryBlock block;
            block.free = true;
            block.offset = 0;
            block.size = size;
            mMem = block.memory = mCore->device().allocateMemory(allocInfo);

            if ((mCore->physicalDevice().getMemoryProperties().memoryTypes[mMemTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
                mPtr = mCore->device().mapMemory(mMem, 0, VK_WHOLE_SIZE);

            mBlocks.emplace_back(block);
        }

        Chunk::Chunk(Chunk && chunk) {
            *this = std::move(chunk);
        }

        Chunk & Chunk::operator=(Chunk && chunk) {
            mCore = chunk.mCore;
            mMem = chunk.mMem;
            mSize = chunk.mSize;
            mMemTypeIndex = chunk.mMemTypeIndex;
            mBlocks = std::move(chunk.mBlocks);
            mPtr = chunk.mPtr;

            chunk.mMem = nullptr;

            return *this;
        }

        Chunk::~Chunk() {
            if (mMem)
                mCore->device().freeMemory(mMem);
        }

        bool Chunk::allocate(vk::DeviceSize size, vk::DeviceSize alignment, MemoryBlock & block) {
            if (size > mSize)
                return false;

            for (size_t i = 0; i < mBlocks.size(); ++i) {
                // if block is unused
                if (mBlocks[i].free) {
                    // compute new size
                    vk::DeviceSize newSize = mBlocks[i].size;

                    // compute new size after being aligned
                    if (mBlocks[i].offset % alignment != 0)
                        newSize -= (alignment - mBlocks[i].offset % alignment);

                    // if block is suitable
                    if (newSize >= size) {
                        mBlocks[i].size = newSize;
                        if (mBlocks[i].offset % alignment != 0)
                            mBlocks[i].offset += (alignment - mBlocks[i].offset % alignment);

                        // compute offset for ptr
                        if (mPtr)
                            mBlocks[i].ptr = static_cast<char*>(mPtr) + mBlocks[i].offset;

                        // if block is perfect match
                        if (mBlocks[i].size == size) {
                            mBlocks[i].free = false;
                            block = mBlocks[i];
                            return true;
                        }

                        // if there are space left
                        MemoryBlock nextBlock;
                        nextBlock.free = true;
                        nextBlock.offset = mBlocks[i].offset + size;
                        nextBlock.memory = mMem;
                        nextBlock.size = mBlocks[i].size - size;
                        mBlocks.emplace_back(nextBlock);

                        mBlocks[i].size = size;
                        mBlocks[i].free = false;

                        block = mBlocks[i];
                        return true;
                    }
                }
            }
            return false;
        }

        void Chunk::deallocate(const MemoryBlock & block) {
            auto it = std::find(mBlocks.begin(), mBlocks.end(), block);
            assert(it != mBlocks.end());

            it->free = true;

            if ((++mOperationCount) >= MaxOperationCount)
                sortOut();
        }

        void Chunk::sortOut() {
            // todo
            mOperationCount = 0;
        }

        std::unique_ptr<Chunk> ChunkFactory::getChunk(vk::DeviceSize size, uint32_t memTypeIndex) {
            size = (mMinChunkSize < size) ? Utils::nextPowerOf2(size) : mMinChunkSize;

            return std::make_unique<Chunk>(mCore, size, memTypeIndex);
        }

        void DeviceAllocator::init(std::shared_ptr<Core> & core) {
            mCore = core;
            mChunkFactory.init(core);
        }

        MemoryBlock DeviceAllocator::allocate(vk::DeviceSize size, vk::DeviceSize alignment, uint32_t memoryTypeIndex) {
            MemoryBlock block;
            // search for a suitable chunk
            for (auto& chunk : mChunks)
                if (chunk->memoryTypeIndex() == memoryTypeIndex)
                    if (chunk->allocate(size, alignment, block))
                        return block;

            // no suitable chunk exist, create one
            mChunks.emplace_back(mChunkFactory.getChunk(size, memoryTypeIndex));
            assert(mChunks.back()->allocate(size, alignment, block));
            return block;
        }

        MemoryBlock DeviceAllocator::allocate(const vk::Image & image,
                                              const vk::MemoryPropertyFlags & properties,
                                              vk::MemoryRequirements* memReq) {
            auto memoryReq = mCore->device().getImageMemoryRequirements(image);
            MemoryBlock block = allocate(memoryReq.size, memoryReq.alignment, mCore->getMemoryTypeIndex(memoryReq.memoryTypeBits, properties));
            mCore->device().bindImageMemory(image, block.memory, block.offset);

            if (memReq)
                *memReq = memoryReq;

            return block;
        }

        MemoryBlock DeviceAllocator::allocate(const vk::Buffer & buffer,
                                              const vk::MemoryPropertyFlags & properties,
                                              vk::MemoryRequirements* memReq) {
            auto memoryReq = mCore->device().getBufferMemoryRequirements(buffer);
            MemoryBlock block = allocate(memoryReq.size, memoryReq.alignment, mCore->getMemoryTypeIndex(memoryReq.memoryTypeBits, properties));
            mCore->device().bindBufferMemory(buffer, block.memory, block.offset);

            if (memReq)
                *memReq = memoryReq;

            return block;
        }

        void DeviceAllocator::deallocate(MemoryBlock & block) {
            for (auto& chunk : mChunks) {
                if (chunk->isIn(block)) {
                    chunk->deallocate(block);
                    return;
                }
            }
            assert(!"Error : unable to deallocate the block");
        }
    }
}

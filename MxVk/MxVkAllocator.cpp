#include "MxVkAllocator.h"

namespace Mix {
    namespace Graphics {
        uint32_t Chunk::MaxOperationCount = 10;

        Chunk::Chunk(std::shared_ptr<Core> _core, vk::DeviceSize _size, uint32_t _memoryTypeIndex) :
            mCore(_core),
            mSize(_size),
            mMemTypeIndex(_memoryTypeIndex) {
            vk::MemoryAllocateInfo allocInfo(_size, _memoryTypeIndex);

            MemoryBlock block;
            block.free = true;
            block.offset = 0;
            block.size = _size;
            mMem = block.memory = mCore->GetDevice().allocateMemory(allocInfo);

            if ((mCore->GetPhysicalDevice().getMemoryProperties().memoryTypes[mMemTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
                mPtr = mCore->GetDevice().mapMemory(mMem, 0, VK_WHOLE_SIZE);

            mBlocks.emplace_back(block);
        }

        Chunk::Chunk(Chunk && _chunk) {
            *this = std::move(_chunk);
        }

        Chunk & Chunk::operator=(Chunk && _chunk) {
            mCore = _chunk.mCore;
            mMem = _chunk.mMem;
            mSize = _chunk.mSize;
            mMemTypeIndex = _chunk.mMemTypeIndex;
            mBlocks = std::move(_chunk.mBlocks);
            mPtr = _chunk.mPtr;

            _chunk.mMem = nullptr;

            return *this;
        }

        Chunk::~Chunk() {
            if (mMem)
                mCore->GetDevice().freeMemory(mMem);
        }

        bool Chunk::Allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, MemoryBlock & _block) {
            if (_size > mSize)
                return false;

            for (size_t i = 0; i < mBlocks.size(); ++i) {
                // if block is unused
                if (mBlocks[i].free) {
                    // compute new size
                    vk::DeviceSize newSize = mBlocks[i].size;

                    // compute new size after being aligned
                    if (mBlocks[i].offset % _alignment != 0)
                        newSize -= (_alignment - mBlocks[i].offset % _alignment);

                    // if block is suitable
                    if (newSize >= _size) {
                        mBlocks[i].size = newSize;
                        if (mBlocks[i].offset % _alignment != 0)
                            mBlocks[i].offset += (_alignment - mBlocks[i].offset % _alignment);

                        // compute offset for ptr
                        if (mPtr)
                            mBlocks[i].ptr = static_cast<char*>(mPtr) + mBlocks[i].offset;

                        // if block is perfect match
                        if (mBlocks[i].size == _size) {
                            mBlocks[i].free = false;
                            _block = mBlocks[i];
                            return true;
                        }

                        // if there are space left
                        MemoryBlock nextBlock;
                        nextBlock.free = true;
                        nextBlock.offset = mBlocks[i].offset + _size;
                        nextBlock.memory = mMem;
                        nextBlock.size = mBlocks[i].size - _size;
                        mBlocks.emplace_back(nextBlock);

                        mBlocks[i].size = _size;
                        mBlocks[i].free = false;

                        _block = mBlocks[i];
                        return true;
                    }
                }
            }
            return false;
        }

        void Chunk::Deallocate(const MemoryBlock & _block) {
            auto it = std::find(mBlocks.begin(), mBlocks.end(), _block);
            assert(it != mBlocks.end());

            it->free = true;

            if ((++mOperationCount) >= MaxOperationCount)
                SortOut();
        }

        void Chunk::SortOut() {
            // todo
            mOperationCount = 0;
        }

        std::unique_ptr<Chunk> ChunkFactory::GetChunk(vk::DeviceSize _size, uint32_t _memTypeIndex) {
            _size = (mMinChunkSize < _size) ? Utils::NextPowerOf2(_size) : mMinChunkSize;

            return std::make_unique<Chunk>(mCore, _size, _memTypeIndex);
        }

        void DeviceAllocator::Init(std::shared_ptr<Core> & _core) {
            mCore = _core;
            mChunkFactory.Init(_core);
        }

        MemoryBlock DeviceAllocator::Allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, uint32_t _memoryTypeIndex) {
            MemoryBlock block;
            // search for a suitable chunk
            for (auto& chunk : mChunks)
                if (chunk->MemoryTypeIndex() == _memoryTypeIndex)
                    if (chunk->Allocate(_size, _alignment, block))
                        return block;

            // no suitable chunk exist, create one
            mChunks.emplace_back(mChunkFactory.GetChunk(_size, _memoryTypeIndex));
            assert(mChunks.back()->Allocate(_size, _alignment, block));
            return block;
        }

        MemoryBlock DeviceAllocator::Allocate(const vk::Image & _image,
                                              const vk::MemoryPropertyFlags & _properties,
                                              vk::MemoryRequirements* memReq) {
            auto memoryReq = mCore->GetDevice().getImageMemoryRequirements(_image);
            MemoryBlock block = Allocate(memoryReq.size, memoryReq.alignment, mCore->GetMemoryTypeIndex(memoryReq.memoryTypeBits, _properties));
            mCore->GetDevice().bindImageMemory(_image, block.memory, block.offset);

            if (memReq)
                *memReq = memoryReq;

            return block;
        }

        MemoryBlock DeviceAllocator::Allocate(const vk::Buffer & _buffer,
                                              const vk::MemoryPropertyFlags & _properties,
                                              vk::MemoryRequirements* memReq) {
            auto memoryReq = mCore->GetDevice().getBufferMemoryRequirements(_buffer);
            MemoryBlock block = Allocate(memoryReq.size, memoryReq.alignment, mCore->GetMemoryTypeIndex(memoryReq.memoryTypeBits, _properties));
            mCore->GetDevice().bindBufferMemory(_buffer, block.memory, block.offset);

            if (memReq)
                *memReq = memoryReq;

            return block;
        }

        void DeviceAllocator::Deallocate(MemoryBlock & _block) {
            for (auto& chunk : mChunks) {
                if (chunk->IsIn(_block)) {
                    chunk->Deallocate(_block);
                    return;
                }
            }
            assert(!"Error : unable to deallocate the block");
        }
    }
}

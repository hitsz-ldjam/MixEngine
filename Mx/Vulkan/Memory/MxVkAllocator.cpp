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
            mMem = block.memory = mCore->getDevice().allocateMemory(allocInfo);

            if ((mCore->getPhysicalDevice().getMemoryProperties().memoryTypes[mMemTypeIndex].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible) == vk::MemoryPropertyFlagBits::eHostVisible)
                mPtr = mCore->getDevice().mapMemory(mMem, 0, VK_WHOLE_SIZE);

            mBlocks.emplace_back(block);
        }

        Chunk::Chunk(Chunk && _chunk) noexcept {
            *this = std::move(_chunk);
        }

        Chunk & Chunk::operator=(Chunk && _chunk) noexcept {
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
                mCore->getDevice().freeMemory(mMem);
        }

        bool Chunk::allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, MemoryBlock & _block) {
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

        void Chunk::deallocate(const MemoryBlock & _block) {
            const auto it = std::find(mBlocks.begin(), mBlocks.end(), _block);
            assert(it != mBlocks.end());

            it->free = true;

            if ((++mOperationCount) >= MaxOperationCount)
                sortOut();
        }

        void Chunk::sortOut() {
            // todo
            mOperationCount = 0;
        }

        std::unique_ptr<Chunk> ChunkFactory::getChunk(vk::DeviceSize _size, uint32_t _memTypeIndex) {
            _size = (mMinChunkSize < _size) ? Math::NextPowerOf2(_size) : mMinChunkSize;

            return std::make_unique<Chunk>(mCore, _size, _memTypeIndex);
        }

        void DeviceAllocator::init(std::shared_ptr<Core> & _core) {
            mCore = _core;
            mChunkFactory.init(_core);
        }

        MemoryBlock DeviceAllocator::allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, uint32_t _memoryTypeIndex) {
            MemoryBlock block;
            // search for a suitable chunk
            for (auto& chunk : mChunks)
                if (chunk->memoryTypeIndex() == _memoryTypeIndex)
                    if (chunk->allocate(_size, _alignment, block))
                        return block;

            // no suitable chunk exist, create one
            mChunks.emplace_back(mChunkFactory.getChunk(_size, _memoryTypeIndex));
            assert(mChunks.back()->allocate(_size, _alignment, block));
            return block;
        }

        MemoryBlock DeviceAllocator::allocate(const vk::Image & _image,
                                              const vk::MemoryPropertyFlags & _properties,
                                              vk::MemoryRequirements* _memReq) {
            auto memoryReq = mCore->getDevice().getImageMemoryRequirements(_image);
            const MemoryBlock block = allocate(memoryReq.size, memoryReq.alignment, mCore->getMemoryTypeIndex(memoryReq.memoryTypeBits, _properties));
            mCore->getDevice().bindImageMemory(_image, block.memory, block.offset);

            if (_memReq)
                *_memReq = memoryReq;

            return block;
        }

        MemoryBlock DeviceAllocator::allocate(const vk::Buffer & _buffer,
                                              const vk::MemoryPropertyFlags & _properties,
                                              vk::MemoryRequirements* _memReq) {
            auto memoryReq = mCore->getDevice().getBufferMemoryRequirements(_buffer);
            const MemoryBlock block = allocate(memoryReq.size, memoryReq.alignment, mCore->getMemoryTypeIndex(memoryReq.memoryTypeBits, _properties));
            mCore->getDevice().bindBufferMemory(_buffer, block.memory, block.offset);

            if (_memReq)
                *_memReq = memoryReq;

            return block;
        }

        void DeviceAllocator::deallocate(MemoryBlock & _block) {
            for (auto& chunk : mChunks) {
                if (chunk->isIn(_block)) {
                    chunk->deallocate(_block);
                    return;
                }
            }
            assert(!"Error : unable to deallocate the block");
        }
    }
}

#pragma once
#ifndef _MX_VK_ALLOCATOR_H_
#define _MX_VK_ALLOCATOR_H_

#include "MxVkCore.h"
#include "../Utils/MxUtils.h"

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

        class Chunk {
        public:
            Chunk(std::shared_ptr<Core> _core, vk::DeviceSize _size, uint32_t _memoryTypeIndex);

            Chunk(const Chunk& _chunk) = delete;
            Chunk(Chunk&& _chunk);

            Chunk& operator=(const Chunk& _chunk) = delete;
            Chunk& operator=(Chunk&& _chunk);

            ~Chunk();

            bool Allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, MemoryBlock& _block);

            bool IsIn(const MemoryBlock& _block) const {
                return mMem == _block.memory;
            }
            void Deallocate(const MemoryBlock& _block);

            uint32_t MemoryTypeIndex() const {
                return mMemTypeIndex;
            }

        private:
            std::shared_ptr<Core> mCore;
            vk::DeviceMemory mMem = nullptr;
            vk::DeviceSize mSize;
            uint32_t mMemTypeIndex;
            std::vector<MemoryBlock> mBlocks;
            void* mPtr = nullptr;

            uint32_t mOperationCount = 0;
            static uint32_t MaxOperationCount;

            void SortOut();
        };

        class ChunkFactory :public GraphicsComponent {
        public:
            void SetMinChunkSize(const vk::DeviceSize _size) {
                assert(Utils::IsPowerOf2(_size));
                mMinChunkSize = _size;
            }

            std::unique_ptr<Chunk> GetChunk(vk::DeviceSize _size, uint32_t _memTypeIndex);

        private:
            vk::DeviceSize mMinChunkSize = 8 * 1024 * 1024;
        };

        class AbstractAllocator :public GraphicsComponent {
        public:
            AbstractAllocator() = default;
            AbstractAllocator(const AbstractAllocator&) = delete;
            AbstractAllocator(AbstractAllocator&& _allocator) {
                *this = std::move(_allocator);
            }

            AbstractAllocator& operator=(const AbstractAllocator&) = delete;
            AbstractAllocator& operator=(AbstractAllocator&& _allocator) {
                mCore = _allocator.mCore;
                return *this;
            }

            virtual MemoryBlock Allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, uint32_t _memoryTypeIndex) = 0;
            virtual void Deallocate(MemoryBlock &_block) = 0;

            virtual ~AbstractAllocator() = default;
        };

        class DeviceAllocator :public AbstractAllocator {
        public:
            void Init(std::shared_ptr<Core>& _core) override;
            MemoryBlock Allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, uint32_t _memoryTypeIndex) override;
            MemoryBlock Allocate(const vk::Image& _image, const vk::MemoryPropertyFlags & _properties, vk::MemoryRequirements* memReq = nullptr);
            MemoryBlock Allocate(const vk::Buffer& _buffer, const vk::MemoryPropertyFlags& _properties, vk::MemoryRequirements* memReq = nullptr);
            void Deallocate(MemoryBlock& _block) override;

        private:
            ChunkFactory mChunkFactory;
            std::vector<std::unique_ptr<Chunk>> mChunks;
        };
    }
}

#endif // !_MX_VK_ALLOCATOR_H_
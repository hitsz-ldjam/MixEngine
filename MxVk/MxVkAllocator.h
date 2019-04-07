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

            bool operator==(const MemoryBlock& block) {
                return (memory == block.memory &&
                        offset == block.offset &&
                        size == block.size &&
                        free == block.free &&
                        ptr == block.ptr);
            }
        };

        class Chunk {
        public:
            Chunk(std::shared_ptr<Core> core, vk::DeviceSize size, uint32_t memoryTypeIndex);

            Chunk(const Chunk& chunk) = delete;
            Chunk(Chunk&& chunk);

            Chunk& operator=(const Chunk& chunk) = delete;
            Chunk& operator=(Chunk&& chunk);

            ~Chunk();

            bool allocate(vk::DeviceSize size, vk::DeviceSize alignment, MemoryBlock& block);

            bool isIn(const MemoryBlock& block) const {
                return mMem == block.memory;
            }
            void deallocate(const MemoryBlock& block);

            uint32_t memoryTypeIndex() const {
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

            void sortOut();
        };

        class ChunkFactory :public GraphicsComponent {
        public:
            void setMinChunkSize(vk::DeviceSize size) {
                assert(Utils::isPowerOf2(size));
                mMinChunkSize = size;
            }

            std::unique_ptr<Chunk> getChunk(vk::DeviceSize size, uint32_t memTypeIndex);

        private:
            vk::DeviceSize mMinChunkSize = 8 * 1024 * 1024;
        };

        class AbstractAllocator :public GraphicsComponent {
        public:
            AbstractAllocator() = default;
            AbstractAllocator(const AbstractAllocator&) = delete;
            AbstractAllocator(AbstractAllocator&& allocator) {
                *this = std::move(allocator);
            }

            AbstractAllocator& operator=(const AbstractAllocator&) = delete;
            AbstractAllocator& operator=(AbstractAllocator&& allocator) {
                mCore = allocator.mCore;
                return *this;
            }

            virtual MemoryBlock allocate(vk::DeviceSize size, vk::DeviceSize alignment, uint32_t memoryTypeIndex) = 0;
            virtual void deallocate(MemoryBlock &block) = 0;

            virtual ~AbstractAllocator() = default;
        };

        class DeviceAllocator :public AbstractAllocator {
        public:
            void init(std::shared_ptr<Core>& core) override;
            MemoryBlock allocate(vk::DeviceSize size, vk::DeviceSize alignment, uint32_t memoryTypeIndex) override;
            MemoryBlock allocate(const vk::Image& image, const vk::MemoryPropertyFlags & properties, vk::MemoryRequirements* memReq = nullptr);
            MemoryBlock allocate(const vk::Buffer& buffer, const vk::MemoryPropertyFlags& properties, vk::MemoryRequirements* memReq = nullptr);
            void deallocate(MemoryBlock& block) override;

        private:
            ChunkFactory mChunkFactory;
            std::vector<std::unique_ptr<Chunk>> mChunks;
        };
    }
}

#endif // !_MX_VK_ALLOCATOR_H_
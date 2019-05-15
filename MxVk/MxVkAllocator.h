#pragma once
#ifndef MX_VK_ALLOCATOR_H_
#define MX_VK_ALLOCATOR_H_

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
            Chunk(Chunk&& _chunk) noexcept;

            Chunk& operator=(const Chunk& _chunk) = delete;
            Chunk& operator=(Chunk&& _chunk) noexcept;

            ~Chunk();

            bool allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, MemoryBlock& _block);

            bool isIn(const MemoryBlock& _block) const {
                return mMem == _block.memory;
            }
            void deallocate(const MemoryBlock& _block);

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
            void setMinChunkSize(const vk::DeviceSize _size) {
                assert(Utils::IsPowerOf2(_size));
                mMinChunkSize = _size;
            }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            std::unique_ptr<Chunk> getChunk(vk::DeviceSize _size, uint32_t _memTypeIndex);

        private:
            vk::DeviceSize mMinChunkSize = 8 * 1024 * 1024;
        };

        class AbstractAllocator :public GraphicsComponent {
            using Base = GraphicsComponent;
        public:
            AbstractAllocator() = default;
            AbstractAllocator(const AbstractAllocator&) = delete;
            AbstractAllocator(AbstractAllocator&& _other) noexcept :Base(std::move(_other)) {
            }

            AbstractAllocator& operator=(const AbstractAllocator&) = delete;

            AbstractAllocator& operator=(AbstractAllocator&& _other) noexcept {
                Base::operator=(std::move(_other));
                return *this; 
            }

            virtual MemoryBlock allocate(vk::DeviceSize _size, vk::DeviceSize _alignment, uint32_t _memoryTypeIndex) = 0;
            virtual void deallocate(MemoryBlock &_block) = 0;

            virtual ~AbstractAllocator() = default;
        };

        class DeviceAllocator :public AbstractAllocator {
        public:
            void init(std::shared_ptr<Core>& _core);

            MemoryBlock allocate(vk::DeviceSize _size, 
                                 vk::DeviceSize _alignment, 
                                 uint32_t _memoryTypeIndex) override;

            MemoryBlock allocate(const vk::Image& _image, 
                                 const vk::MemoryPropertyFlags & _properties, 
                                 vk::MemoryRequirements* _memReq = nullptr);

            MemoryBlock allocate(const vk::Buffer& _buffer, 
                                 const vk::MemoryPropertyFlags& _properties, 
                                 vk::MemoryRequirements* _memReq = nullptr);

            void deallocate(MemoryBlock& _block) override;

        private:
            ChunkFactory mChunkFactory;
            std::vector<std::unique_ptr<Chunk>> mChunks;
        };
    }
}

#endif // !MX_VK_ALLOCATOR_H_
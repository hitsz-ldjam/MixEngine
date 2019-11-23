#pragma once
#ifndef MX_GPU_BUFFER_H_
#define MX_GPU_BUFFER_H_
#include <cstdint>
#include "../../Demos/DFJ/Scripts/PlayerShip.h"

namespace Mix {
    struct GPUBufferDesc {
        uint32_t size;
        GPUBufferUsage usage;
    };


    class GPUBuffer {
    public:
        virtual ~GPUBuffer() = default;

        virtual void getData(void* _dst, uint32_t _offset, uint32_t _size, uint32_t _queueIdx = 0) = 0;

        virtual void setData(void* _src, uint32_t _offset, uint32_t _size, uint32_t _queueIdx = 0) = 0;

        virtual void copyData(GPUBuffer& _src, uint32_t _srcOffset, uint32_t _dstOffset, uint32_t _size) = 0;

        virtual void copyData(GPUBuffer& _src) = 0;

        uint32_t size() const { return mSize; }

        bool isMapped() const { return mIsMapped; }

        void* map(uint32_t _offset, uint32_t _size, uint32_t _queueIdx = 0);

        void unmap();

        static std::shared_ptr<GPUBuffer> Create(const GPUBufferDesc& _desc);

    protected:
        GPUBuffer(const GPUBufferDesc& _desc) :mSize(_desc.size), mUsage(_desc.usage), mIsMapped(false) {}

        virtual void* mapInternal(uint32_t _offset, uint32_t _size, uint32_t _queueIdx = 0) = 0;

        virtual void unmapInternal() = 0;

        uint32_t mSize;
        GPUBufferUsage mUsage;
        bool mIsMapped;
    };
}

#endif

#pragma once
#ifndef MX_INDEX_BUFFER_H_
#define MX_INDEX_BUFFER_H_
#include "../Definitions/MxCommonEnum.h"
#include "../Definitions/MxTypes.h"
#include "MxGPUBuffer.h"
#include <memory>

namespace Mix {
    struct IndexBufferDesc {
        IndexFormat format = IndexFormat::UInt16;
        uint32 indexCount = 0;
        GPUBufferUsage usage = GPUBufferUsage::Static;
    };


    class IndexBuffer :public GPUBuffer {
    public:
        static std::shared_ptr<IndexBuffer> Create(const IndexBufferDesc& _desc);

        IndexFormat getIndexFormat() const { return mIndexFormat; }

        uint32 getIndexCount() const { return mIndexCount; }

    protected:
        IndexBuffer(const IndexBufferDesc& _desc);

        IndexFormat mIndexFormat;
        uint32 mIndexCount;
    };

}

#endif
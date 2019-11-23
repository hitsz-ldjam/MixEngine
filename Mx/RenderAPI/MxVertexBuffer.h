#pragma once
#ifndef MX_VERTEX_BUFFER_H_
#define MX_VERTEX_BUFFER_H_
#include "../Definitions/MxTypes.h"
#include "MxGPUBuffer.h"

namespace Mix {
    struct VertexBufferDesc {
        uint32 vertexSize = 0;
        uint32 vertexCount = 0;
        GPUBufferUsage usage = GPUBufferUsage::Static;
    };


    class VertexBuffer :public GPUBuffer {
    public:
        static std::shared_ptr<VertexBuffer> Create(const VertexBufferDesc& _desc);

        uint32 getVertexSize() const { return mVertexSize; }

        uint32 getVertexCount() const { return mVertexCount; }

    protected:
        VertexBuffer(const VertexBufferDesc& _desc) :
            GPUBuffer({ _desc.vertexCount*_desc.vertexSize, _desc.usage }),
            mVertexSize(_desc.vertexSize),
            mVertexCount(_desc.vertexCount) {
        }

        uint32 mVertexSize;
        uint32 mVertexCount;
    };
}

#endif

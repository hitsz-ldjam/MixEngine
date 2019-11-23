#include "MxIndexBuffer.h"
#include "MxRenderAPI.h"

namespace Mix {

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(const IndexBufferDesc& _desc) {
        return RenderAPI::Get()->createIndexBuffer(_desc);
    }

    IndexBuffer::IndexBuffer(const IndexBufferDesc& _desc) :
        GPUBuffer({ _desc.indexCount*Utils::GetIndexFormatSize(_desc.format), _desc.usage }),
        mIndexFormat(_desc.format),
        mIndexCount(_desc.indexCount) {
    }

}

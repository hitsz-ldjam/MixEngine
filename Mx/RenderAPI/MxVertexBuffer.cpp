#include "MxVertexBuffer.h"
#include "MxRenderAPI.h"

namespace Mix {
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(const VertexBufferDesc& _desc) {
        return RenderAPI::Get()->createVertexBuffer(_desc);
    }
}

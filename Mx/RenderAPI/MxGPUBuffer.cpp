#include "MxGPUBuffer.h"
#include "MxRenderAPI.h"

namespace Mix {


    void* GPUBuffer::map(uint32_t _offset, uint32_t _size, uint32_t _queueIdx) {
        MX_ASSERT(!isMapped() && "Cannot map this buffer that has already been mapped, unmap it first.");
        void* p = mapInternal(_offset, _size, _queueIdx);
        mIsMapped = true;
        return p;
    }

    void GPUBuffer::unmap() {
        MX_ASSERT(isMapped() && "Cannot unmap this buffer that has not been mapped.");
        unmapInternal();
        mIsMapped = false;
    }

    std::shared_ptr<GPUBuffer> GPUBuffer::Create(const GPUBufferDesc& _desc) {
        return RenderAPI::Get()->createGPUBuffer(_desc);
    }

}

#pragma once
#ifndef MX_VK_GPUBUFFER_H_
#define MX_VK_GPUBUFFER_H_
#include "../../RenderAPI/MxGPUBuffer.h"

namespace Mix {
    namespace Vulkan {
        class GPUBuffer :public Mix::GPUBuffer {
        public:
            ~GPUBuffer();

            void getData(void* _dst, uint32_t _offset, uint32_t _size, uint32_t _queueIdx) override;

            void setData(void* _src, uint32_t _offset, uint32_t _size, uint32_t _queueIdx) override;

            void copyData(Mix::GPUBuffer& _src, uint32_t _srcOffset, uint32_t _dstOffset, uint32_t _size) override;

            void copyData(Mix::GPUBuffer& _src) override;

        protected:
            void* mapInternal(uint32_t _offset, uint32_t _size, uint32_t _queueIdx) override;

            void unmapInternal() override;

        };
    }
}

#endif

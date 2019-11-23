#include "MxCommandBuffer.h"
#include "MxRenderAPI.h"

namespace Mix {
    std::shared_ptr<CommandBuffer> CommandBuffer::Create(GPUQueueType _type, uint32_t _queueIdx, bool _secondary) {
        return RenderAPI::Get()->createCommandBuffer(_type, _queueIdx, _secondary);
    }

    CommandBuffer::CommandBuffer(GPUQueueType _type, uint32_t _queueIdx, bool _secondary) :
        mType(_type),
        mQueueIdx(_queueIdx),
        mIsSecondary(_secondary) {
    }
}

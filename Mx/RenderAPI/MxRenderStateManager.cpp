#include "MxRenderStateManager.h"
#include "../../MixEngine.h"
#include "MxGPUParams.h"

namespace Mix {

    RenderStateManager* RenderStateManager::Get() {
        return MixEngine::Instance().getModule<RenderStateManager>();
    }

    std::shared_ptr<PipelineParamsInfo> RenderStateManager::createPipelineParamsInfo(const GraphicsParamsDesc& _desc) const {
        return std::shared_ptr<PipelineParamsInfo>(new PipelineParamsInfo(_desc));
    }

    std::shared_ptr<PipelineParamsInfo> RenderStateManager::createPipelineParamsInfo(const ComputeParamsDesc& _desc) const {
        return std::shared_ptr<PipelineParamsInfo>(new PipelineParamsInfo(_desc));
    }
}

#pragma once
#ifndef MX_RENDER_STATE_MANAGER_H_
#define MX_RENDER_STATE_MANAGER_H_
#include "../Engine/MxModuleBase.h"


namespace Mix {
    class PipelineParamsInfo;
    struct GraphicsParamsDesc;
    struct ComputeParamsDesc;

    class RenderStateManager :public ModuleBase {
    public:
        static RenderStateManager* Get();

        RenderStateManager() = default;

        virtual std::shared_ptr<PipelineParamsInfo> createPipelineParamsInfo(const GraphicsParamsDesc& _desc) const;

        virtual std::shared_ptr<PipelineParamsInfo> createPipelineParamsInfo(const ComputeParamsDesc& _desc) const;

    private:
    };
}

#endif

#pragma once
#ifndef MX_GPU_PIPELINE_STATE_H_
#define MX_GPU_PIPELINE_STATE_H_
#include "MxBlendState.h"
#include "MxRasterizerState.h"
#include "MxDepthStencilState.h"
#include <optional>
#include "MxGPUParam.h"

namespace Mix {
    class GPUProgram;

    struct GraphicsPipelineStateDesc {
        TargetBlendStateDesc blendState;
        RasterizerStateDesc rasterizerState;
        DepthStencilStateDesc depthStencilState;

        struct {
            std::shared_ptr<GPUProgram> vertex;
            std::shared_ptr<GPUProgram> fragment;
            std::shared_ptr<GPUProgram> geometry;
            std::shared_ptr<GPUProgram> tessEvaluation;
            std::shared_ptr<GPUProgram> tessControl;
        }programs;
    };

    struct GPUParamLocation {
        uint32 set = uint32(-1);
        uint32 binding = uint32(-1);
    };


    class GraphicsPipelineState {
    public:
        virtual ~GraphicsPipelineState() = default;

        bool hasVertexProgram() const { return mDesc.programs.vertex != nullptr; }

        bool hasFragmentProgram() const { return mDesc.programs.fragment != nullptr; }

        bool hasGeometryProgram() const { return mDesc.programs.geometry != nullptr; }

        bool hasTessEvaluationProgram() const { return mDesc.programs.tessEvaluation != nullptr; }

        bool hasTessControlProgram() const { return mDesc.programs.tessControl != nullptr; }

        const TargetBlendStateDesc& getBlendState() const { return mDesc.blendState; }

        const RasterizerStateDesc& getRasterizerState() const { return mDesc.rasterizerState; }

        const DepthStencilStateDesc& getDepthStencilState() const { return mDesc.depthStencilState; }

        std::shared_ptr<GPUProgram> getVertexProgram() const { return mDesc.programs.vertex; }

        std::shared_ptr<GPUProgram> getFragmentProgram() const { return mDesc.programs.fragment; }

        std::shared_ptr<GPUProgram> getGeometryProgram() const { return mDesc.programs.geometry; }

        std::shared_ptr<GPUProgram> getTessEvaluationProgram() const { return mDesc.programs.tessEvaluation; }

        std::shared_ptr<GPUProgram> getTessControlProgram() const { return mDesc.programs.tessControl; }


    protected:
        GraphicsPipelineState(const GraphicsPipelineStateDesc& _desc);

        static std::shared_ptr<GraphicsPipelineState> Create(const GraphicsPipelineStateDesc& _desc);

        GraphicsPipelineStateDesc mDesc;
    };
}

#endif


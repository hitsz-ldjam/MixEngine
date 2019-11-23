#pragma once
#ifndef MX_COMMANDBUFFER_H_
#define MX_COMMANDBUFFER_H_

#include "../Utils/MxGeneralBase.hpp"
#include "../Definitions/MxCommonEnum.h"
#include "../Utils/MxArrayProxy.h"
#include <optional>
#include "../Math/MxColor.h"

namespace Mix {
    enum class CommandBufferState {
        Empty,
        Recording,
        Executing,
        Complete
    };

    class GPUParams;
    class GraphicsPipelineState;
    class VertexBuffer;
    class VertexDeclaration;
    class Viewport;
    class RenderTarget;
    class Color;
    struct Rect2f;

    class CommandBuffer : public GeneralBase::NoCopyBase {
    public:
        virtual ~CommandBuffer() = default;


        GPUQueueType getQueueType() const { return mType; }

        uint32_t getQueueIdx() const { return mQueueIdx; }

        virtual CommandBufferState getState() const = 0;

        virtual void reset() = 0;

        virtual void setRenderTarget(const std::shared_ptr<RenderTarget>& _target) = 0;

        virtual void clearRenderTarget(const Color& _color = Color::Black,
                                       float depth = 1.0f, uint32 stencil = 0,
                                       std::optional<Viewport> _viewport = std::nullopt) = 0;

        virtual void setGraphicsPipeline(const std::shared_ptr<GraphicsPipelineState>& _pipeline) = 0;

        virtual void setGPUParams(const std::shared_ptr<GPUParams>& _gpuParams) = 0;

        virtual void setVertexBuffer(uint32 _index, ArrayProxy<const std::shared_ptr<VertexBuffer>> _buffers) = 0;

        virtual void setIndexBuffer(const std::shared_ptr<VertexBuffer>& _buffers) = 0;

        virtual void setVertexDeclaration(const std::shared_ptr<VertexDeclaration>& _vertexDecl) = 0;

        virtual void draw(uint32 _vertexOffset, uint32 _vertexCount, uint32 _instanceCount = 1) = 0;

        virtual void draw(uint32 _indexOffset, uint32 _indexCount, 
                          uint32 _vertexOffset, uint32 _vertexCount,
                          uint32 _instanceCount = 1) = 0;

        virtual void setViewport(const Viewport& _viewport) = 0;

        virtual void setScissors(const Rect2f& _scissor) = 0;

        virtual void setStencilRef(uint32 _value) = 0;

        virtual void appendCommands(ArrayProxy<const std::shared_ptr<CommandBuffer>> _commands) = 0;

        virtual void submit() = 0;

        static std::shared_ptr<CommandBuffer> Create(GPUQueueType _type, uint32_t _queueIdx = 0, bool _secondary = false);


    protected:
        CommandBuffer(GPUQueueType _type, uint32_t _queueIdx, bool _secondary);

        GPUQueueType mType;
        uint32_t mQueueIdx;
        bool mIsSecondary;
    };
}

#endif

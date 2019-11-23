#pragma once
#ifndef MX_RENDER_API_H_
#define MX_RENDER_API_H_

#include <string>
#include "MxRenderTarget.h"
#include "MxCommandBuffer.h"
#include "MxGPUProgramManager.h"

namespace Mix {
    class RenderAPIManager;
    class GPUBuffer;
    struct GPUBufferDesc;
    class VertexBuffer;
    struct VertexBufferDesc;
    class IndexBuffer;
    struct IndexBufferDesc;
    struct GraphicsPipelineStateDesc;

    class RenderAPI {
    public:
        static RenderAPI* Get();

        virtual ~RenderAPI() = default;

        virtual std::string getApiName() const = 0;

        virtual std::shared_ptr<CommandBuffer> createCommandBuffer(GPUQueueType _type, uint32_t _queueIdx = 0, bool _secondary = false) = 0;

        virtual std::shared_ptr<GPUBuffer> createGPUBuffer(const GPUBufferDesc& _desc) = 0;

        virtual std::shared_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc& _desc) = 0;

        virtual std::shared_ptr<IndexBuffer> createIndexBuffer(const IndexBufferDesc& _desc) = 0;

        virtual std::shared_ptr<GraphicsPipelineState> createGraphicsPipeline(const GraphicsPipelineStateDesc& _desc) = 0;

        std::shared_ptr<GPUProgram> createGPUProgram(const GPUProgramDesc& _desc) const;

        GPUProgramManager& getGPUProgramManager() { return mGPUProgramManager; }

        /*virtual void setRenderTarget(const std::shared_ptr<RenderTarget>& _target,
                                     const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void clearRenderTarget(const Color& _color = Color::Black,
                                       float depth = 1.0f, uint32 stencil = 0,
                                       std::optional<Viewport> _viewport = std::nullopt,
                                       const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void setGraphicsPipeline(const std::shared_ptr<GraphicsPipelineState>& _pipeline,
                                         const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void setVertexBuffer(uint32 _index, ArrayProxy<const std::shared_ptr<VertexBuffer>> _buffers,
                                     const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void setIndexBuffer(const std::shared_ptr<VertexBuffer>& _buffers,
                                    const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void setVertexDeclaration(const std::shared_ptr<VertexDeclaration>& _vertexDecl,
                                          const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void draw(uint32 _vertexOffset, uint32 _vertexCount, uint32 _instanceCount = 1,
                          const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void draw(uint32 _indexOffset, uint32 _indexCount, uint32 _vertexOffset, uint32 _vertexCount,
                          uint32 _instanceCount = 1, const std::shared_ptr<CommandBuffer>& _cmd = nullptr) = 0;

        virtual void submit(const std::shared_ptr<CommandBuffer>& _cmd) = 0;*/

        virtual std::shared_ptr<CommandBuffer> getMainCmdBuffer() const = 0;

    protected:
        friend RenderAPIManager;

        RenderAPI() = default;

        virtual void init() = 0;

        virtual void destroy() = 0;

        std::shared_ptr<RenderTarget> mActiveRenderTarget;
        bool renderTargetChanged = false;

        GPUProgramManager mGPUProgramManager;
    };

}

#endif

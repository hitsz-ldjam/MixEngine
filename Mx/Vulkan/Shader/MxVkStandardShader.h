#pragma once
#ifndef MX_VK_STANDARD_RENDERER_H_
#define MX_VK_STANDARD_RENDERER_H_

#include "MxVkShaderBase.h"
#include "../Buffers/MxVkUniformBuffer.h"
#include "../FrameBuffer/MxVkFramebuffer.h"
#include <deque>
#include "../Pipeline/MxVkGraphicsPipelineState.h"

namespace Mix {
    class Ui;
    class Renderer;


    namespace Vulkan {
        class Pipeline;
        class RenderPass;
        class Image;
        class Swapchain;
        class DescriptorSetLayout;
        class DescriptorPool;

        class StandardShader final : public ShaderBase {
        public:
            explicit StandardShader(VulkanAPI* _vulkan);

            ~StandardShader() override;

            void render(Shader& _shader, const SceneRenderInfo& _renderInfo) override;

            uint32_t newMaterial() override;

            void deleteMaterial(uint32_t _id) override;

        private:
            std::shared_ptr<Device> mDevice;
            std::shared_ptr<Swapchain> mSwapchain;

            std::shared_ptr<GraphicsPipelineState> mGraphicsPipelineState;
            std::shared_ptr<RenderPass> mRenderPass;

            std::vector<FrameBuffer> mFrameBuffers;

            std::shared_ptr<Image> mDepthStencil;
            vk::ImageView mDepthStencilView;

            std::shared_ptr<DescriptorSetLayout> mStaticParamDescriptorSetLayout;
            std::shared_ptr<DescriptorSetLayout> mDynamicPamramDescriptorSetLayout;
            std::shared_ptr<DescriptorPool> mDescriptorPool;
            std::vector<DescriptorSet> mStaticDescriptorSets;
            std::vector<Buffer> mCameraUniforms;
            std::vector<DynamicUniformBuffer> mDynamicUniform;

            uint32_t mDefaultMaterialCount = 10;
            std::unordered_map<std::string, uint32_t> mMaterialNameBindingMap;
            std::vector<std::vector<DescriptorSet>> mMaterialDescs;
            std::deque<uint32_t> mUnusedId;

            // test GUI
            /*Ui* mUi = nullptr;
            vk::DescriptorSet mGuiDescriptorSet;

            std::shared_ptr<Pipeline> mGuiPipeline;*/

            void buildRenderPass();
            void buildFrameBuffer();
            void buildDescriptorSetLayout();
            void buildPipeline();
            void buildDescriptorSet();
            void buildPropertyBlock();

            void beginFrame(Shader& _shader, const SceneRenderInfo& _renderInfo);
            void setCamera(const Camera& _camera);
            void beginRenderer(const Renderer& _renderer);
            void endRenderer(const Renderer& _renderer);
            bool choosePipeline(const Mesh& _mesh, uint32_t _submesh);
            void updateShaderParam(Shader& _shader);
            void setMaterailParam(Material& _material);
            void endFrame();

            // Frame rendering info
            std::shared_ptr<VertexInput> mCurrVertexInput;
            std::shared_ptr<Pipeline> mCurrPipeline;
            uint32_t mCurrFrame = 0;
            CommandBufferHandle* mCurrCmd;
        };
    }
}

#endif

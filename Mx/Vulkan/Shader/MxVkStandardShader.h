#pragma once
#ifndef MX_VK_STANDARD_RENDERER_H_
#define MX_VK_STANDARD_RENDERER_H_

#include "MxVkShaderBase.h"
#include "../Buffers/MxVkUniformBuffer.h"
#include "../FrameBuffer/MxVkFramebuffer.h"
#include "../Descriptor/MxVkDescriptorSet.h"
#include <deque>
#include "../Pipeline/MxVkGraphicsPipelineState.h"

namespace Mix {
    class GUI;
    class Renderer;


    namespace Vulkan {
        class Pipeline;
        class RenderPass;
        class Image;
        class Swapchain;
        class DescriptorSetLayout;
        class DescriptorPool;
        class DescriptorSet;

        class StandardShader final : public ShaderBase {
        public:
            explicit StandardShader(VulkanAPI* _vulkan);

            ~StandardShader() override;

            void render(ArrayProxy<RenderQueueElement> _elements) override;

            void update(const Shader& _shader) override;

            void beginRender(const Camera& _camera) override;

            void endRender() override;

            uint32_t newMaterial() override;

            void deleteMaterial(uint32_t _id) override;

        private:
            void setCamera(const Camera& _camera);

            void beginElement(const RenderElement& _element);

            void endElement();

            bool choosePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh);

            void updateMaterial(Material& _material);

            void setMaterail(Material& _material);


            std::shared_ptr<Device> mDevice;

            std::shared_ptr<GraphicsPipelineState> mGraphicsPipeline;
            std::shared_ptr<GraphicsPipelineState> mInstanceGraphicsPipeline;

            std::shared_ptr<DescriptorSetLayout> mStaticParamDescriptorSetLayout;
            std::shared_ptr<DescriptorSetLayout> mDynamicPamramDescriptorSetLayout;
            std::shared_ptr<DescriptorPool> mDescriptorPool;
            std::vector<DescriptorSet> mStaticDescriptorSets;
            std::vector<Buffer> mCameraUniforms;
            std::vector<DynamicUniformBuffer> mDynamicUniform;

            uint32_t mDefaultMaterialCount = 100;
            std::unordered_map<std::string, uint32_t> mMaterialNameBindingMap;
            std::vector<std::vector<DescriptorSet>> mMaterialDescs;
            std::deque<uint32_t> mUnusedId;

            // test GUI
            /*Ui* mUi = nullptr;
            vk::DescriptorSet mGuiDescriptorSet;

            std::shared_ptr<Pipeline> mGuiPipeline;*/

            // void buildRenderPass();
            // void buildFrameBuffer();
            void buildDescriptorSetLayout();
            void buildPipeline();
            void buildDescriptorSet();
            void buildPropertyBlock();

            // Frame rendering info
            std::shared_ptr<VertexInput> mCurrVertexInput;
            std::shared_ptr<Pipeline> mCurrPipeline;
            uint32_t mCurrFrame = 0;
            CommandBufferHandle* mCurrCmd;
        };
    }
}

#endif

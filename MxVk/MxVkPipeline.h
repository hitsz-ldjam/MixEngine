#pragma once
#ifndef _MX_VK_PIPELINE_H_
#define _MX_VK_PIPELINE_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class Pipeline :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            void destroy();

            virtual ~Pipeline() { destroy(); }

            Pipeline & operator=(Pipeline && pipeline);

            virtual void init(const Core* core) override;

            void setTargetRenderPass(const vk::RenderPass renderPass, const uint32_t subpassIndex);

            void addShader(const vk::ShaderStageFlagBits stage,
                           const vk::ShaderModule shader,
                           const vk::SpecializationInfo* specInfo = nullptr);

            void setVertexInput(const std::vector<vk::VertexInputBindingDescription>& bindingDescri,
                                const std::vector<vk::VertexInputAttributeDescription>& attriDescri);

            void setInputAssembly(const vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList,
                                  const bool primitiveRestart = false);

            void addViewport(const std::vector<vk::Viewport>& viewports);
            void addViewport(const vk::Viewport& viewport);
            void addScissor(const std::vector<vk::Rect2D>& scissors);
            void addScissor(const vk::Rect2D& scissors);

            void setRasterization(const vk::PolygonMode polygonMode, const vk::CullModeFlags cullMode, const vk::FrontFace frontFace,
                                  const float lineWidth = 1.0f,
                                  const bool depthClampEnable = false, const bool rasterizerDiscardEnable = false);

            void setDepthBias(const bool enable,
                              const float constantFactor = 0.0f,
                              const float slopeFactor = 0.0f,
                              const float clamp = 0.0f);

            void setMultiSample(const vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
                                const bool sampleShading = false, const float minSampleShading = 0.0f,
                                const vk::SampleMask* sampleMask = nullptr,
                                const bool alphaToCoverageEnable = false, const bool alphaToOneEnable = false);

            void setDepthTest(const bool depthTestEnable,
                              const bool depthWriteEnable = true,
                              const vk::CompareOp depthCompareOp = vk::CompareOp::eLess);

            void setDepthBoundsTest(const bool enable, const float minBounds = 0.0f, const float maxBounds = 0.0f);

            void setStencilTest(const bool enable,
                                const vk::StencilOpState& front = {}, const vk::StencilOpState& back = {});

            void addDefaultBlendAttachments();
            void addBlendAttachments(const vk::PipelineColorBlendAttachmentState& attachment);
            void addBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& attachments);

            void setBlend(const bool logicalOpEnable = false, const vk::LogicOp logicOp = vk::LogicOp::eCopy,
                          const float constantR = 0.0, const float constantG = 0.0,
                          const float constantB = 0.0, const float constantA = 0.0);

            void addDynamicState(const vk::DynamicState dynamicState);
            void addDynamicState(const std::vector<vk::DynamicState>& dynamicStates);

            void addDescriptorSetLayout(const vk::DescriptorSetLayout setLayout);
            void addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& setLayouts);

            void addPushConstantRange(vk::ShaderStageFlags stageFlags, uint32_t offset, uint32_t size);
            void addPushConstantRange(const vk::PushConstantRange& range);
            void addPushConstantRanges(const std::vector<vk::PushConstantRange>& ranges = std::vector<vk::PushConstantRange>());

            bool create();

            vk::Pipeline get() const { return mPipeline; };
            vk::PipelineLayout pipelineLayout() const { return mPipelineLayout; };

        private:
            vk::Pipeline mPipeline;
            vk::PipelineLayout mPipelineLayout;
            vk::RenderPass mRenderPass;
            uint32_t mSubpassIndex = 0;

            struct PipelineStates {
                std::vector<vk::PipelineShaderStageCreateInfo> shaders;
                vk::PipelineVertexInputStateCreateInfo vertexInput;
                vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
                std::vector<vk::Viewport> viewports;
                std::vector<vk::Rect2D> scissors;
                vk::PipelineRasterizationStateCreateInfo rasterization;
                vk::PipelineMultisampleStateCreateInfo multisample;
                vk::PipelineDepthStencilStateCreateInfo depthStencil;
                std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
                vk::PipelineColorBlendStateCreateInfo colorBlend;
                std::vector<vk::DynamicState> dynamicStates;
                std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
                std::vector<vk::PushConstantRange> pushConstantRanges;
            }*mPipelineStates = nullptr;

            void clear();
        };

        class PipelineMgr :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        private:
            std::map<std::string, Pipeline> mPipelines;

        public:
            Pipeline& createPipeline(const std::string& name,
                                     const vk::RenderPass renderPass = 0,
                                     const uint32_t subpassIndex = 0);

            const Pipeline& getPipeline(const std::string& name);

            void destroyPipeline(const std::string& name);

            void destroy();

            ~PipelineMgr() { destroy(); };
        };
    }
}
#endif // !_MX_VK_PIPELINE_H_

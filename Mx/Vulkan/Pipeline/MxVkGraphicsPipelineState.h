#pragma once
#ifndef MX_VK_GRAPHICS_PIPELINE_STATE_H_
#define MX_VK_GRAPHICS_PIPELINE_STATE_H_
#include <map>
#include <vulkan/vulkan.hpp>
#include "../../Utils/MxArrayProxy.h"
#include "../../Definitions/MxCommonEnum.h"
#include <unordered_map>

namespace Mix {
    class VertexDeclaration;
    namespace Vulkan {
        class Device;
        class ShaderModule;
        class DescriptorSetLayout;
        class Pipeline;
        class RenderPass;
        class VertexInput;

        struct GraphicsPipelineStateDesc {
            struct {
                std::shared_ptr<ShaderModule> vertex;
                std::shared_ptr<ShaderModule> geometry;
                std::shared_ptr<ShaderModule> tessControl;
                std::shared_ptr<ShaderModule> tessEvaluation;
                std::shared_ptr<ShaderModule> fragment;
            }gpuProgram;

            vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
            vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack;
            vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;

            bool enableDepthTest;
            bool enableWriteDepth;

            std::shared_ptr<VertexDeclaration> vertexDecl;
            std::vector<vk::PipelineColorBlendAttachmentState> blendStates;
            std::vector<vk::PushConstantRange> pushConstant;
            std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;
        };

        class GraphicsPipelineState {
        public:
            explicit GraphicsPipelineState(std::shared_ptr<Device> _device, const GraphicsPipelineStateDesc& _desc);

            ~GraphicsPipelineState();

            std::shared_ptr<VertexDeclaration> getVertexDeclaration() const { return mVertexDecl; }

            const vk::PipelineLayout& getPipelineLayout() const;

            std::shared_ptr<Pipeline> getPipeline(const std::shared_ptr<RenderPass>& _renderPass,
                                                  uint32_t _subpassIndex,
                                                  const std::shared_ptr<VertexInput>& _vertexInput,
                                                  MeshTopology _drawMode = MeshTopology::Triangles_List,
                                                  bool _depthTest = true,
                                                  bool _depthWrite = true,
                                                  bool _stencilTest = false);

            static const vk::PipelineColorBlendAttachmentState DefaultBlendAttachment;
        private:
            struct PipelineStateData {
                vk::PipelineShaderStageCreateInfo shaderStageInfo[5];
                vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
                vk::PipelineViewportStateCreateInfo viewportInfo;
                vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
                vk::PipelineMultisampleStateCreateInfo multisampleInfo;
                vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
                vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
                vk::PipelineTessellationStateCreateInfo tesselationInfo;
                vk::PipelineDynamicStateCreateInfo dynamicInfo;
                vk::PipelineLayout pipelineLayout;
                vk::GraphicsPipelineCreateInfo pipelineCreateInfo;

                std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
                std::vector<vk::DynamicState> dynamicStates;
                std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;
                std::vector<vk::PushConstantRange> pushConstantRanges;
                PipelineStateData();
            };

            struct PipelineKey {
                uint32_t renderPassId;
                uint32_t subpass;
                MeshTopology drawMode;
                uint32_t vertexInputId;
                bool depthTest;
                bool depthWrite;
                bool stencilTest;

                static size_t Hash(const PipelineKey& _v);

                struct Hasher {
                    bool operator()(const PipelineKey& _key) const;
                };

                bool operator==(const PipelineKey& _other) const;

                bool operator!=(const PipelineKey& _other) const;
            };

            PipelineStateData mPipelineStateData;
            std::shared_ptr<Device> mDevice;
            std::shared_ptr<VertexDeclaration> mVertexDecl;
            std::vector<std::shared_ptr<ShaderModule>> mShaderModules;
            std::unordered_map<PipelineKey, std::shared_ptr<Pipeline>, PipelineKey::Hasher> mPipelineMap;


            std::shared_ptr<Pipeline> createPipeline(const std::shared_ptr<RenderPass>& _renderPass,
                                                     uint32_t _subpassIndex,
                                                     MeshTopology _drawMode,
                                                     const std::shared_ptr<VertexInput>& _vertexInput,
                                                     bool _depthTest = true,
                                                     bool _depthWrite = true,
                                                     bool _stencilTest = false);
        };
    }
}

#endif

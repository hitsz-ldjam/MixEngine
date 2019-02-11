#pragma once
#ifndef _MX_VK_PIPELINE_H_
#define _MX_VK_PIPELINE_H_

#include"MxVkComponent.h"

#include<vector>

namespace Mix
{
	class MxVkPipeline :public MxVkComponent
	{
	private:
		VkPipeline mPipeline;
		VkPipelineLayout mPipelineLayout;
		VkRenderPass mRenderPass;
		uint32_t mSubpassIndex;

		struct PipelineStates
		{
			std::vector<VkPipelineShaderStageCreateInfo> shaders;
			VkPipelineVertexInputStateCreateInfo vertexInput;
			VkPipelineInputAssemblyStateCreateInfo inputAssembly;
			std::vector<VkViewport> viewports;
			std::vector<VkRect2D> scissors;
			VkPipelineRasterizationStateCreateInfo rasterization;
			VkPipelineMultisampleStateCreateInfo multisample;
			VkPipelineDepthStencilStateCreateInfo depthStencil;
			std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
			VkPipelineColorBlendStateCreateInfo colorBlend;
			std::vector<VkDynamicState> dynamicStates;
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			std::vector<VkPushConstantRange> pushConstantRanges;
		}*mPipelineStates;

		void clear();
	public:
		MxVkPipeline();
		bool setup(const MxVkManager* manager);

		void setTargetRenderPass(const VkRenderPass renderPass, const uint32_t subpassIndex);

		void addShader(const VkShaderStageFlagBits stage, const VkShaderModule shader);

		void setVertexInput(const std::vector<VkVertexInputBindingDescription>& bindingDescri,
							const std::vector<VkVertexInputAttributeDescription>& attriDescri);

		void setInputAssembly(const VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, const bool primitiveRestart = false);

		void addViewport(const std::vector<VkViewport>& viewports);
		void addViewport(const VkViewport& viewport);
		void addScissor(const std::vector<VkRect2D>& scissors);
		void addScissor(const VkRect2D& scissors);

		void setRasterization(const VkPolygonMode polygonMode, const VkCullModeFlags cullMode, const VkFrontFace frontFace,
							  const float lineWidth = 1.0f,
							  const bool depthClampEnable = false, const bool rasterizerDiscardEnable = false);

		void setDepthBias(const bool enable,
						  const float constantFactor = 0.0f,
						  const float slopeFactor = 0.0f,
						  const float clamp = 0.0f);

		void setMultiSample(const VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
							const bool sampleShading = false, const float minSampleShading = 0.0f,
							const VkSampleMask* sampleMask = nullptr,
							const bool alphaToCoverageEnable = false, const bool alphaToOneEnable = false);

		void setDepthTest(const bool depthTestEnable,
						  const bool depthWriteEnable = true,
						  const VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS);

		void setDepthBoundsTest(const bool enable, const float minBounds = 0.0f, const float maxBounds = 0.0f);

		void setStencilTest(const bool enable,
							const VkStencilOpState& front = {}, const VkStencilOpState& back = {});

		void addDefaultBlendAttachments();
		void addBlendAttachments(const VkPipelineColorBlendAttachmentState& attachment);
		void addBlendAttachments(const std::vector<VkPipelineColorBlendAttachmentState>& attachments);

		void setBlend(const bool logicalOpEnable = false, const VkLogicOp logicOp = VK_LOGIC_OP_COPY,
					  const float constantR = 0.0, const float constantG = 0.0,
					  const float constantB = 0.0, const float constantA = 0.0);

		void addDynamicState(const VkDynamicState dynamicState);
		void addDynamicState(const std::vector<VkDynamicState>& dynamicStates);

		void addDescriptorSetLayout(const VkDescriptorSetLayout setLayout);
		void addDescriptorSetLayout(const std::vector<VkDescriptorSetLayout>& setLayouts);

		void addPushConstantRanges(const VkPushConstantRange& range);
		void addPushConstantRanges(const std::vector<VkPushConstantRange>& ranges = std::vector<VkPushConstantRange>());

		bool createPipeline();

		VkPipeline getPipeline() const { return mPipeline; };
		VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; };

		void destroy();

		~MxVkPipeline();
	};
}

#endif // !_MX_VK_PIPELINE_H_

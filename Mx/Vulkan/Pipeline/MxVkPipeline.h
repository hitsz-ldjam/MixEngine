#pragma once
#ifndef MX_VK_PIPELINE_H_
#define MX_VK_PIPELINE_H_

#include "MxVkRenderPass.h"
#include "../../Utils/MxArrayProxy.h"
#include <map>

namespace nlohmann {
	class json;
}

namespace Mix {
	namespace Graphics {
		class Shader;
		class DescriptorSetLayout;

		class Pipeline : public GeneralBase::NoCopyBase {
			friend class PipelineFactory;
		public:
			/*explicit Pipeline(const std::shared_ptr<RenderPass>& _renderPass, const uint32_t _subpassIndex)
				: mRenderPass(_renderPass),
				mSubpassIndex(_subpassIndex),
				mPipelineStates(new PipelineStates{}) {
			}*/

			Pipeline(Pipeline&& _other) noexcept { swap(_other); }

			Pipeline& operator=(Pipeline&& _other) noexcept { swap(_other); return *this; }

			void swap(Pipeline& _other) noexcept;

			/*void addShader(const Shader& _shader, const vk::SpecializationInfo* _specInfo = nullptr) const;

			void setVertexInput(const std::vector<vk::VertexInputBindingDescription>& _bindingDescri,
								const std::vector<vk::VertexInputAttributeDescription>& _attriDescri) const;

			void setInputAssembly(const vk::PrimitiveTopology _topology = vk::PrimitiveTopology::eTriangleList,
								  const bool _primitiveRestart = false) const;

			void addViewport(std::initializer_list<vk::Viewport> _viewports) const;

			void addViewport(const vk::Viewport& _viewport) const;

			void addScissor(std::initializer_list<vk::Rect2D> _scissors) const;

			void addScissor(const vk::Rect2D& _scissors) const;

			void setRasterization(const vk::PolygonMode _polygonMode,
								  const vk::CullModeFlags _cullMode,
								  const vk::FrontFace _frontFace,
								  const float _lineWidth = 1.0f,
								  const bool _depthClampEnable = false,
								  const bool _rasterizerDiscardEnable = false) const;

			void setDepthBias(const bool _enable,
							  const float _constantFactor = 0.0f,
							  const float _slopeFactor = 0.0f,
							  const float _clamp = 0.0f) const;

			void setMultiSample(const vk::SampleCountFlagBits _samples = vk::SampleCountFlagBits::e1,
								const bool _sampleShading = false,
								const float _minSampleShading = 0.0f,
								const vk::SampleMask* _sampleMask = nullptr,
								const bool _alphaToCoverageEnable = false,
								const bool _alphaToOneEnable = false) const;

			void setDepthTest(const bool _depthTestEnable,
							  const bool _depthWriteEnable = true,
							  const vk::CompareOp _depthCompareOp = vk::CompareOp::eLess) const;

			void setDepthBoundsTest(const bool _enable, const float _minBounds = 0.0f,
									const float _maxBounds = 0.0f) const;

			void setStencilTest(const bool _enable = false,
								const vk::StencilOpState& _front = {},
								const vk::StencilOpState& _back = {}) const;

			void addDefaultBlendAttachment() const;

			void addBlendAttachments(const vk::PipelineColorBlendAttachmentState& _attachment) const;

			void addBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& _attachments) const;

			void setBlend(const bool _logicalOpEnable = false,
						  const vk::LogicOp _logicOp = vk::LogicOp::eCopy,
						  const float _constantR = 0.0,
						  const float _constantG = 0.0,
						  const float _constantB = 0.0,
						  const float _constantA = 0.0) const;

			void addDynamicState(const vk::DynamicState _dynamicState) const;

			void addDynamicState(const std::vector<vk::DynamicState>& _dynamicStates) const;

			void addDescriptorSetLayout(const vk::DescriptorSetLayout _setLayout) const;

			void addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& _setLayouts) const;

			void addPushConstantRange(vk::ShaderStageFlags _stageFlags, uint32_t _offset, uint32_t _size) const;

			void addPushConstantRange(const vk::PushConstantRange& _range) const;

			void addPushConstantRanges(const std::vector<vk::PushConstantRange>& _ranges) const;

			bool create();*/

			const vk::Pipeline& get() const { return mPipeline; };

			const vk::PipelineLayout& pipelineLayout() const { return mPipelineLayout; }

			const std::vector<std::shared_ptr<DescriptorSetLayout>>& descriptorSetLayouts() const { return mDescriptorSetLayout; }


		private:
			Pipeline(std::shared_ptr<RenderPass> _renderPass,
					 ArrayProxy<std::shared_ptr<DescriptorSetLayout>> _descriptorSetLayout,
					 const uint32_t _subpassIndex,
					 const vk::Pipeline& _pipeline,
					 const vk::PipelineLayout& _layout);

			std::shared_ptr<RenderPass> mRenderPass;
			std::vector<std::shared_ptr<DescriptorSetLayout>> mDescriptorSetLayout;
			vk::Pipeline mPipeline;
			vk::PipelineLayout mPipelineLayout;
			uint32_t mSubpassIndex = 0;

			/*struct PipelineStates {
				std::vector<vk::PipelineShaderStageCreateInfo> shaders;
				vk::PipelineVertexInputStateCreateInfo vertexInput;
				vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {},vk::PrimitiveTopology::eTriangleList };
				std::vector<vk::Viewport> viewports;
				std::vector<vk::Rect2D> scissors;
				vk::PipelineRasterizationStateCreateInfo rasterization = { {},false,false,vk::PolygonMode::eFill,vk::CullModeFlagBits::eBack,vk::FrontFace::eCounterClockwise,false,0.0f,0.0f,0.0f,1.0f };
				vk::PipelineMultisampleStateCreateInfo multisample;
				vk::PipelineDepthStencilStateCreateInfo depthStencil;
				std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
				vk::PipelineColorBlendStateCreateInfo colorBlend;
				std::vector<vk::DynamicState> dynamicStates;
				std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
				std::vector<vk::PushConstantRange> pushConstantRanges;
			};

			std::shared_ptr<PipelineStates> mPipelineStates;
			void clear();*/
		};

		/*class PipelineMgr : public GeneralBase::NoCopyBase {
		public:
			std::shared_ptr<Pipeline> createPipeline(const std::string& _name,
													 const std::shared_ptr<RenderPass>& _renderPass,
													 const uint32_t _subpassIndex);

			std::shared_ptr<Pipeline> getPipeline(const std::string& _name);

			void destroyPipeline(const std::string& _name);

			void destroy();

			~PipelineMgr() { destroy(); }

		private:
			std::map<std::string, std::shared_ptr<Pipeline>> mPipelines;
		};*/
	}
}
#endif // !MX_VK_PIPELINE_H_

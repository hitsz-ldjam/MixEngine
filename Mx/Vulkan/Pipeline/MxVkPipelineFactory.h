#pragma once
#ifndef MX_VK_PIPELINE_FACTORY_H_
#define MX_VK_PIPELINE_FACTORY_H_

#include "../../Utils/MxArrayProxy.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <map>

namespace nlohmann {
	class json;
}

namespace Mix {
	namespace Graphics {
		class Pipeline;
		class RenderPass;
		class Shader;
		class DescriptorSetLayout;
		class Device;

		class PipelineFactory {
		public:
			static std::shared_ptr<Pipeline> CreatePipelineFromJson(const std::shared_ptr<RenderPass>& _renderPass,
																	const uint32_t _subpassIndex,
																	const nlohmann::json& _json);

			void begin();

			void setShader(const Shader& _shader, const vk::SpecializationInfo* _specInfo = nullptr) const;

			void setVertexInput(ArrayProxy<const vk::VertexInputBindingDescription> _bindingDescri,
								ArrayProxy<const vk::VertexInputAttributeDescription> _attriDescri) const;

			void setInputAssembly(const vk::PrimitiveTopology _topology = vk::PrimitiveTopology::eTriangleList,
								  const bool _primitiveRestart = false) const;

			void setViewport(ArrayProxy<const vk::Viewport> _viewports) const;

			void setScissor(ArrayProxy<const vk::Rect2D> _scissors) const;

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

			void setBlendAttachments(ArrayProxy<const vk::PipelineColorBlendAttachmentState> _attachments) const;

			void setBlend(const bool _logicalOpEnable = false,
						  const vk::LogicOp _logicOp = vk::LogicOp::eCopy,
						  const float _constantR = 0.0,
						  const float _constantG = 0.0,
						  const float _constantB = 0.0,
						  const float _constantA = 0.0) const;

			void setDynamicState(ArrayProxy<const vk::DynamicState> _dynamicStates) const;

			void setDescriptorSetLayout(ArrayProxy<std::shared_ptr<DescriptorSetLayout>> _setLayouts) const;

			void setPushConstantRanges(ArrayProxy<const vk::PushConstantRange> _ranges) const;

			std::shared_ptr<Pipeline> createPipeline(const std::shared_ptr<RenderPass>& _renderPass,
													 const uint32_t _subpassIndex);
			void clear();

			static const vk::PipelineColorBlendAttachmentState DefaultBlendAttachment;

		private:
			static std::vector<Shader> CreateShaderStageFromJson(const nlohmann::json& _json, const std::shared_ptr<Device>& _device);
			static void CreateVertexInputFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateInputAssemblyFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateRasterizationFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateDepthStencilFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateBlendFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateDynamicStateFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static void CreateDescriptorSetLayoutFromJson(const nlohmann::json& _json, PipelineFactory& _factory);
			static vk::Format GetVkFormatFromJson(const std::string& _str);

			struct PipelineStates {
				std::map<vk::ShaderStageFlagBits, vk::PipelineShaderStageCreateInfo> shaders;
				std::vector<vk::VertexInputBindingDescription> inputBindings;
				std::vector<vk::VertexInputAttributeDescription> inputAttributes;
				vk::PipelineInputAssemblyStateCreateInfo inputAssembly = { {},vk::PrimitiveTopology::eTriangleList };
				std::vector<vk::Viewport> viewports;
				std::vector<vk::Rect2D> scissors;
				vk::PipelineRasterizationStateCreateInfo rasterization = { {},false,false,vk::PolygonMode::eFill,vk::CullModeFlagBits::eBack,vk::FrontFace::eCounterClockwise,false,0.0f,0.0f,0.0f,1.0f };
				vk::PipelineMultisampleStateCreateInfo multisample;
				vk::PipelineDepthStencilStateCreateInfo depthStencil;
				std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
				vk::PipelineColorBlendStateCreateInfo colorBlend;
				std::vector<vk::DynamicState> dynamicStates;
				std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;
				std::vector<vk::PushConstantRange> pushConstantRanges;
			};

			std::shared_ptr<PipelineStates> mPipelineStates;

			static const char* const sDefaultEntry;
		};
	}
}

#endif

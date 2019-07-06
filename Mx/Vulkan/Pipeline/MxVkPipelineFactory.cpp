#include "MxVkPipelineFactory.h"
#include "MxVkPipeline.h"
#include "MxVkShader.h"
#include "../../../MixEngine.h"
#include "../../Resource/MxResourceLoader.h"
#include "../Descriptor/MxVkDescriptor.h"

namespace Mix {
	namespace Graphics {
		const char* const PipelineFactory::sDefaultEntry = "main";

		const vk::PipelineColorBlendAttachmentState PipelineFactory::DefaultBlendAttachment = {
			true,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA
		};

		std::vector<Shader> PipelineFactory::CreateShaderStageFromJson(const nlohmann::json& _json, const std::shared_ptr<Device>& _device) {
			std::vector<Shader> results;
			for (auto& path : _json) {
				results.emplace_back(_device, MixEngine::Instance().getModule<Resource::ResourceLoader>()->load<Resource::ShaderSource>(path.get<std::string>()));
			}
			return results;
		}

		void PipelineFactory::CreateVertexInputFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			std::vector<vk::VertexInputBindingDescription> bindings;
			std::vector<vk::VertexInputAttributeDescription> attributes;

			uint32_t bd = 0, lc = 0;
			for (auto& bindingJs : _json) {
				vk::VertexInputBindingDescription binding;
				binding.stride = bindingJs["stride"].get<uint32_t>();
				binding.inputRate =
					bindingJs["inputRate"].get<std::string>() == std::string("vertex") ?
					vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance;

				auto& attributeJs = bindingJs["attribute"];
				for (auto& attri : attributeJs) {
					vk::Format format = GetVkFormatFromJson(attri["format"].get<std::string>());
					uint32_t offset = attri["offset"].get<uint32_t>();
					attributes.emplace_back(lc++, bd, format, offset);
				}

				binding.binding = bd++;
				bindings.push_back(binding);
			}

			_factory.setVertexInput(bindings, attributes);
		}

		void PipelineFactory::CreateInputAssemblyFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
			auto str = _json["topology"].get<std::string>();

			if (str == "PointList")				topology = vk::PrimitiveTopology::ePointList;
			else if (str == "LineList")			topology = vk::PrimitiveTopology::eLineList;
			else if (str == "LineStrip")		topology = vk::PrimitiveTopology::eLineStrip;
			else if (str == "TriangleList")		topology = vk::PrimitiveTopology::eTriangleList;
			else if (str == "TriangleStrip")	topology = vk::PrimitiveTopology::eTriangleStrip;
			else if (str == "TriangleFan")		topology = vk::PrimitiveTopology::eTriangleFan;

			bool restart = false;
			if (_json.contains("primitiveRestart"))
				restart = _json["primitiveRestart"].get<bool>();

			_factory.setInputAssembly(topology, restart);
		}

		void PipelineFactory::CreateRasterizationFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			vk::PolygonMode polygon = vk::PolygonMode::eFill;
			if (_json.contains("polygon")) {
				auto str = _json["polygon"].get<std::string>();

				if (str == "Fill");
				else if (str == "Line") polygon = vk::PolygonMode::eLine;
				else if (str == "Point")polygon = vk::PolygonMode::ePoint;
			}

			vk::CullModeFlags cullMode = vk::CullModeFlagBits::eBack;
			if (_json.contains("cullMode")) {
				auto str = _json["cullMode"].get<std::string>();

				if (str == "Back");
				if (str == "Front") cullMode = vk::CullModeFlagBits::eFront;
				if (str == "FrontAndBack")cullMode = vk::CullModeFlagBits::eFrontAndBack;
				if (str == "eNone")cullMode = vk::CullModeFlagBits::eNone;
			}

			vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;
			if (_json.contains("frontFace")) {
				auto str = _json["frontFace"].get<std::string>();

				if (str == "ClockWise") frontFace = vk::FrontFace::eClockwise;
			}

			float lineWidth = 1.0f;
			if (_json.contains("lineWidth")) {
				lineWidth = _json["lineWidth"].get<float>();
			}

			bool depthClamp = false;
			if (_json.contains("depthClamp")) {
				depthClamp = _json.get<bool>();
			}

			bool discard = false;
			if (_json.contains("discard")) {
				discard = _json["discard"].get<bool>();
			}

			// depthBias
			bool enable = false;
			float constantFactor = 0.0f;
			float clamp = 0.0f;
			float slopeFactor = 0.0f;
			if (_json.contains("depthBias")) {
				auto& depthBiasJs = _json["depthBias"];
				enable = depthBiasJs["enable"].get<bool>();
				constantFactor = depthBiasJs["constantFactor"].get<float>();
				clamp = depthBiasJs["clamp"].get<float>();
				slopeFactor = depthBiasJs["slopeFactor"].get<float>();
			}

			_factory.setRasterization(polygon, cullMode, frontFace, lineWidth, depthClamp, discard);
			_factory.setDepthBias(enable, constantFactor, clamp, slopeFactor);
		}

		void PipelineFactory::CreateDepthStencilFromJson(const nlohmann::json& _json, PipelineFactory& _factory) {
			{
				// depthTest
				auto& depthTestJs = _json["depthTest"];
				bool enable = true;
				bool writeEnable = true;
				auto compareOp = vk::CompareOp::eLess;
			}
		}

		vk::Format PipelineFactory::GetVkFormatFromJson(const std::string& _str) {
			if (_str == "R32_Sfloat")	return vk::Format::eR32Sfloat;
			if (_str == "R32G32_Sfloat")	return vk::Format::eR32G32Sfloat;
			if (_str == "R32G32B32_Sfloat")	return vk::Format::eR32G32B32Sfloat;
			if (_str == "R32G32B32A32_Sfloat")	return vk::Format::eR32G32B32A32Sfloat;
			if (_str == "R32_Int")	return vk::Format::eR32Sint;
			if (_str == "R32G32_Int")	return vk::Format::eR32G32Sint;
			if (_str == "R32G32B32_Int")	return vk::Format::eR32G32B32Sfloat;
			if (_str == "R32G32B32A32_Int")	return vk::Format::eR32G32B32A32Sfloat;
			if (_str == "R32_Uint")	return vk::Format::eR32Uint;
			if (_str == "R32G32_Uint")	return vk::Format::eR32G32Uint;
			if (_str == "R32G32B32_Uint")	return vk::Format::eR32G32B32Uint;
			if (_str == "R32G32B32A32_Uint")	return vk::Format::eR32G32B32A32Uint;
			if (_str == "R8_Unorm")	return vk::Format::eR8Unorm;
			if (_str == "R8G8_Unorm")	return vk::Format::eR8G8Unorm;
			if (_str == "R8G8B8_Unorm")	return vk::Format::eR8G8B8Unorm;
			if (_str == "R8G8B8A8_Unorm")	return vk::Format::eR8G8B8A8Unorm;
			return vk::Format::eUndefined;
		}

		std::shared_ptr<Pipeline> PipelineFactory::CreatePipelineFromJson(const std::shared_ptr<RenderPass>& _renderPass,
																		  const uint32_t _subpassIndex,
																		  const nlohmann::json& _json) {
			PipelineFactory factory;
			factory.begin();
			auto shaders = std::move(CreateShaderStageFromJson(_json["shaderStage"], _renderPass->getDevice()));
			for (auto& shader : shaders)
				factory.setShader(shader);

			CreateVertexInputFromJson(_json["vertexInput"], factory);
			CreateInputAssemblyFromJson(_json["inputAssembly"], factory);
			CreateRasterizationFromJson(_json["rasterization"], factory);
			CreateDepthStencilFromJson(_json["depthStencil"], factory);
			CreateBlendFromJson(_json["blend"], factory);
			CreateDynamicStateFromJson(_json["dynamicState"], factory);
			CreateDescriptorSetLayoutFromJson(_json["descriptorSetLayout"], factory);
		}

		void PipelineFactory::begin() {
			mPipelineStates.reset(new PipelineStates);
		}

		void PipelineFactory::setShader(const Shader& _shader, const vk::SpecializationInfo* _specInfo) const {
			if (mPipelineStates) {
				mPipelineStates->shaders[_shader.stage()] = { {}, _shader.stage(), _shader.get(), sDefaultEntry, _specInfo };
			}
		}

		void PipelineFactory::setVertexInput(ArrayProxy<const vk::VertexInputBindingDescription> _bindingDescri,
											 ArrayProxy<const vk::VertexInputAttributeDescription> _attriDescri) const {
			if (mPipelineStates) {
				mPipelineStates->inputBindings.assign(_bindingDescri.begin(), _bindingDescri.end());
				mPipelineStates->inputAttributes.assign(_attriDescri.begin(), _attriDescri.end());
			}
		}

		void PipelineFactory::setInputAssembly(const vk::PrimitiveTopology _topology,
											   const bool _primitiveRestart) const {
			if (mPipelineStates) {
				mPipelineStates->inputAssembly.topology = _topology;
				mPipelineStates->inputAssembly.primitiveRestartEnable = _primitiveRestart;
			}
		}

		void PipelineFactory::setViewport(ArrayProxy<const vk::Viewport> _viewports) const {
			if (mPipelineStates)
				mPipelineStates->viewports.assign(_viewports.begin(), _viewports.end());
		}

		void PipelineFactory::setScissor(ArrayProxy<const vk::Rect2D> _scissors) const {
			if (mPipelineStates)
				mPipelineStates->scissors.assign(_scissors.begin(), _scissors.end());
		}

		void PipelineFactory::setRasterization(const vk::PolygonMode _polygonMode,
											   const vk::CullModeFlags _cullMode,
											   const vk::FrontFace _frontFace,
											   const float _lineWidth,
											   const bool _depthClampEnable,
											   const bool _rasterizerDiscardEnable) const {
			if (mPipelineStates) {

				mPipelineStates->rasterization.polygonMode = _polygonMode;
				mPipelineStates->rasterization.cullMode = _cullMode;
				mPipelineStates->rasterization.frontFace = _frontFace;
				mPipelineStates->rasterization.lineWidth = _lineWidth;
				mPipelineStates->rasterization.depthClampEnable = _depthClampEnable;
				mPipelineStates->rasterization.rasterizerDiscardEnable = _rasterizerDiscardEnable;
			}
		}

		void PipelineFactory::setDepthBias(const bool _enable,
										   const float _constantFactor,
										   const float _slopeFactor,
										   const float _clamp) const {
			if (mPipelineStates) {
				mPipelineStates->rasterization.depthBiasEnable = _enable;
				mPipelineStates->rasterization.depthBiasConstantFactor = _constantFactor;
				mPipelineStates->rasterization.depthBiasSlopeFactor = _slopeFactor;
				mPipelineStates->rasterization.depthBiasClamp = _clamp;
			}
		}

		void PipelineFactory::setMultiSample(const vk::SampleCountFlagBits _samples,
											 const bool _sampleShading,
											 const float _minSampleShading,
											 const vk::SampleMask* _sampleMask,
											 const bool _alphaToCoverageEnable,
											 const bool _alphaToOneEnable) const {
			if (mPipelineStates) {
				mPipelineStates->multisample.sampleShadingEnable = _sampleShading;
				mPipelineStates->multisample.rasterizationSamples = _samples;
				mPipelineStates->multisample.minSampleShading = _minSampleShading;
				mPipelineStates->multisample.pSampleMask = _sampleMask;
				mPipelineStates->multisample.alphaToCoverageEnable = _alphaToCoverageEnable;
				mPipelineStates->multisample.alphaToOneEnable = _alphaToOneEnable;
			}
		}

		void PipelineFactory::setDepthTest(const bool _depthTestEnable,
										   const bool _depthWriteEnable,
										   const vk::CompareOp _depthCompareOp) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.depthTestEnable = _depthTestEnable;
				mPipelineStates->depthStencil.depthWriteEnable = _depthWriteEnable;
				mPipelineStates->depthStencil.depthCompareOp = _depthCompareOp;
			}
		}

		void PipelineFactory::setDepthBoundsTest(const bool _enable,
												 const float _minBounds,
												 const float _maxBounds) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.depthBoundsTestEnable = _enable;
				mPipelineStates->depthStencil.minDepthBounds = _minBounds;
				mPipelineStates->depthStencil.maxDepthBounds = _maxBounds;
			}
		}

		void PipelineFactory::setStencilTest(const bool _enable,
											 const vk::StencilOpState& _front,
											 const vk::StencilOpState& _back) const {
			if (mPipelineStates) {
				mPipelineStates->depthStencil.stencilTestEnable = _enable;
				mPipelineStates->depthStencil.front = _front;
				mPipelineStates->depthStencil.back = _back;
			}
		}

		void PipelineFactory::addDefaultBlendAttachment() const {
			if (mPipelineStates) {
				mPipelineStates->colorBlendAttachments.push_back(DefaultBlendAttachment);
			}
		}

		void PipelineFactory::setBlendAttachments(ArrayProxy<const vk::PipelineColorBlendAttachmentState> _attachments) const {
			if (mPipelineStates) {
				mPipelineStates->colorBlendAttachments.assign(_attachments.begin(), _attachments.end());
			}
		}

		void PipelineFactory::setBlend(const bool _logicalOpEnable,
									   const vk::LogicOp _logicOp,
									   const float _constantR,
									   const float _constantG,
									   const float _constantB,
									   const float _constantA) const {
			if (mPipelineStates) {
				mPipelineStates->colorBlend.logicOpEnable = _logicalOpEnable;
				mPipelineStates->colorBlend.logicOp = _logicOp;
				mPipelineStates->colorBlend.blendConstants[0] = _constantR;
				mPipelineStates->colorBlend.blendConstants[1] = _constantG;
				mPipelineStates->colorBlend.blendConstants[2] = _constantB;
				mPipelineStates->colorBlend.blendConstants[3] = _constantA;
			}
		}

		void PipelineFactory::setDynamicState(ArrayProxy<const vk::DynamicState> _dynamicStates) const {
			if (mPipelineStates)
				mPipelineStates->dynamicStates.assign(_dynamicStates.begin(), _dynamicStates.end());
		}

		void PipelineFactory::setDescriptorSetLayout(ArrayProxy<std::shared_ptr<DescriptorSetLayout>> _setLayouts) const {
			if (mPipelineStates)
				mPipelineStates->descriptorSetLayouts.assign(_setLayouts.begin(), _setLayouts.end());
		}

		void PipelineFactory::setPushConstantRanges(ArrayProxy<const vk::PushConstantRange> _ranges) const {
			if (mPipelineStates)
				mPipelineStates->pushConstantRanges.assign(_ranges.begin(), _ranges.end());
		}

		std::shared_ptr<Pipeline> PipelineFactory::createPipeline(const std::shared_ptr<RenderPass>& _renderPass,
																  const uint32_t _subpassIndex) {
			// create pipeline layout
			std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
			descriptorSetLayouts.reserve(mPipelineStates->descriptorSetLayouts.size());
			for (auto& layout : mPipelineStates->descriptorSetLayouts)
				descriptorSetLayouts.emplace_back(layout->get());

			vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
			layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
			layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			layoutCreateInfo.pPushConstantRanges = mPipelineStates->pushConstantRanges.data();
			layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(mPipelineStates->pushConstantRanges.size());

			auto pipelineLayout = _renderPass->getDevice()->get().createPipelineLayout(layoutCreateInfo);

			vk::PipelineViewportStateCreateInfo viewportState = {};
			viewportState.pViewports = mPipelineStates->viewports.data();
			viewportState.viewportCount = static_cast<uint32_t>(mPipelineStates->viewports.size());
			viewportState.pScissors = mPipelineStates->scissors.data();
			viewportState.scissorCount = static_cast<uint32_t>(mPipelineStates->scissors.size());

			if (mPipelineStates->colorBlendAttachments.empty())
				addDefaultBlendAttachment();

			mPipelineStates->colorBlend.pAttachments = mPipelineStates->colorBlendAttachments.data();
			mPipelineStates->colorBlend.attachmentCount = static_cast<uint32_t>(mPipelineStates->colorBlendAttachments.size());

			vk::PipelineDynamicStateCreateInfo dynamicState = {};
			dynamicState.pDynamicStates = mPipelineStates->dynamicStates.data();
			dynamicState.dynamicStateCount = static_cast<uint32_t>(mPipelineStates->dynamicStates.size());

			std::vector<vk::PipelineShaderStageCreateInfo> stages;
			stages.reserve(mPipelineStates->shaders.size());
			for (auto& pair : mPipelineStates->shaders) {
				stages.emplace_back(pair.second);
			}

			vk::PipelineVertexInputStateCreateInfo inputState;
			inputState.pVertexBindingDescriptions = mPipelineStates->inputBindings.data();
			inputState.vertexBindingDescriptionCount = static_cast<uint32_t>(mPipelineStates->inputBindings.size());
			inputState.pVertexAttributeDescriptions = mPipelineStates->inputAttributes.data();
			inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(mPipelineStates->inputAttributes.size());

			vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
			pipelineCreateInfo.pStages = stages.data();
			pipelineCreateInfo.stageCount = static_cast<uint32_t>(stages.size());
			pipelineCreateInfo.pVertexInputState = &inputState;
			pipelineCreateInfo.pInputAssemblyState = &mPipelineStates->inputAssembly;
			pipelineCreateInfo.pViewportState = &viewportState;
			pipelineCreateInfo.pRasterizationState = &mPipelineStates->rasterization;
			pipelineCreateInfo.pMultisampleState = &mPipelineStates->multisample;
			pipelineCreateInfo.pDepthStencilState = &mPipelineStates->depthStencil;
			pipelineCreateInfo.pColorBlendState = &mPipelineStates->colorBlend;

			if (dynamicState.dynamicStateCount == 0)
				pipelineCreateInfo.pDynamicState = nullptr;
			else
				pipelineCreateInfo.pDynamicState = &dynamicState;

			pipelineCreateInfo.layout = pipelineLayout;
			pipelineCreateInfo.renderPass = _renderPass->get();
			pipelineCreateInfo.subpass = _subpassIndex;
			pipelineCreateInfo.basePipelineHandle = nullptr;	//基础管线（vulkan允许在已经存在的管线上派生新的管线）
			pipelineCreateInfo.basePipelineIndex = -1;

			auto pipeline = _renderPass->getDevice()->get().createGraphicsPipeline(nullptr, pipelineCreateInfo);

			return std::shared_ptr<Pipeline>(new Pipeline(_renderPass, mPipelineStates->descriptorSetLayouts, _subpassIndex, pipeline, pipelineLayout));
		}

		void PipelineFactory::clear() {
			mPipelineStates.reset();
		}

	}
}

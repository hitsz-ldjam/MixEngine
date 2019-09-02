#include "MxVkGraphicsPipelineState.h"
#include "../MxVkUtils.h"
#include "../Descriptor/MxVkDescriptorSet.h"
#include "MxVkShaderModule.h"
#include "MxVkPipeline.h"
#include "MxVkVertexInput.h"

namespace Mix {
	namespace Vulkan {
		const vk::PipelineColorBlendAttachmentState GraphicsPipelineState::DefaultBlendAttachment = {
			false,
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

		GraphicsPipelineState::GraphicsPipelineState(std::shared_ptr<Device> _device, const GraphicsPipelineStateDesc& _desc) :mDevice(std::move(_device)) {
			// Prepare shader stage
			std::pair<vk::ShaderStageFlagBits, std::shared_ptr<ShaderModule>> shaderModules[] = {
				{vk::ShaderStageFlagBits::eVertex,					_desc.gpuProgram.vertex},
				{vk::ShaderStageFlagBits::eGeometry,				_desc.gpuProgram.geometry},
				{vk::ShaderStageFlagBits::eTessellationControl,		_desc.gpuProgram.tessControl},
				{vk::ShaderStageFlagBits::eTessellationEvaluation,	_desc.gpuProgram.tessEvaluation},
				{vk::ShaderStageFlagBits::eFragment,				_desc.gpuProgram.fragment}
			};

			uint32_t usedStageCount = 0;
			uint32_t count = std::end(shaderModules) - std::begin(shaderModules);
			std::vector<std::shared_ptr<ShaderModule>> usedShaderModules;
			for (uint32_t i = 0; i < count; ++i) {
				if (!shaderModules[i].second)
					continue;

				usedShaderModules.push_back(shaderModules[i].second);
				auto &ci = mPipelineStateData.shaderStageInfo[usedStageCount];
				ci.stage = shaderModules[i].first;
				ci.module = shaderModules[i].second->get();
				ci.pName = "main";

				++usedStageCount;
			}

			mVertexDecl = _desc.vertexDecl;
			mShaderModules = std::move(usedShaderModules);

			// Prepare input assebly state
			mPipelineStateData.inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList; // Set at runtime

			// Prepare viewport state
			mPipelineStateData.viewportInfo.pViewports = nullptr; // Use dynamic state
			mPipelineStateData.viewportInfo.pScissors = nullptr;
			mPipelineStateData.viewportInfo.viewportCount = 1;
			mPipelineStateData.viewportInfo.scissorCount = 1;

			// Prepare rasterization state
			{
				mPipelineStateData.rasterizationInfo.depthClampEnable = false;
				mPipelineStateData.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
				mPipelineStateData.rasterizationInfo.polygonMode = _desc.polygonMode;
				mPipelineStateData.rasterizationInfo.cullMode = _desc.cullMode;
				mPipelineStateData.rasterizationInfo.frontFace = _desc.frontFace;
				mPipelineStateData.rasterizationInfo.depthBiasEnable = false;
				mPipelineStateData.rasterizationInfo.depthBiasConstantFactor = 0.0f;
				mPipelineStateData.rasterizationInfo.depthBiasSlopeFactor = 0.0f;
				mPipelineStateData.rasterizationInfo.depthBiasClamp = 0.0f;
				mPipelineStateData.rasterizationInfo.lineWidth = 1.0f;
			}

			// Prepare multisample state
			{
				mPipelineStateData.multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1; // Make this optional
				mPipelineStateData.multisampleInfo.sampleShadingEnable = VK_FALSE;
				mPipelineStateData.multisampleInfo.minSampleShading = 1.0f;
				mPipelineStateData.multisampleInfo.pSampleMask = nullptr;
				mPipelineStateData.multisampleInfo.alphaToCoverageEnable = false;
				mPipelineStateData.multisampleInfo.alphaToOneEnable = VK_FALSE;
			}

			// Prepare depth stencil info
			{
				vk::StencilOpState stencilInfo;
				stencilInfo.compareOp = vk::CompareOp::eLess;
				stencilInfo.depthFailOp = vk::StencilOp::eKeep;
				stencilInfo.passOp = vk::StencilOp::eKeep;
				stencilInfo.failOp = vk::StencilOp::eKeep;
				stencilInfo.compareMask = 0;
				stencilInfo.writeMask = 0;

				mPipelineStateData.depthStencilInfo.depthBoundsTestEnable = false;
				mPipelineStateData.depthStencilInfo.minDepthBounds = 0.0f;
				mPipelineStateData.depthStencilInfo.maxDepthBounds = 1.0f;
				mPipelineStateData.depthStencilInfo.depthTestEnable = _desc.enableDepthTest;
				mPipelineStateData.depthStencilInfo.depthWriteEnable = _desc.enableWriteDepth;
				mPipelineStateData.depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
				mPipelineStateData.depthStencilInfo.front = stencilInfo;
				mPipelineStateData.depthStencilInfo.back = stencilInfo;
				mPipelineStateData.depthStencilInfo.stencilTestEnable = false;
			}

			// Prepare color blend state info
			{
				mPipelineStateData.colorBlendAttachments = _desc.blendStates;

				mPipelineStateData.colorBlendInfo.logicOp = vk::LogicOp::eNoOp;
				mPipelineStateData.colorBlendInfo.logicOpEnable = VK_FALSE;
				mPipelineStateData.colorBlendInfo.attachmentCount = static_cast<uint32_t>(mPipelineStateData.colorBlendAttachments.size());
				mPipelineStateData.colorBlendInfo.pAttachments = mPipelineStateData.colorBlendAttachments.data();
				mPipelineStateData.colorBlendInfo.blendConstants[0] = 0.0f;
				mPipelineStateData.colorBlendInfo.blendConstants[1] = 0.0f;
				mPipelineStateData.colorBlendInfo.blendConstants[2] = 0.0f;
				mPipelineStateData.colorBlendInfo.blendConstants[3] = 0.0f;
			}


			// Prepare dynamic state info
			{
				mPipelineStateData.dynamicStates.push_back(vk::DynamicState::eViewport);
				mPipelineStateData.dynamicStates.push_back(vk::DynamicState::eScissor);

				mPipelineStateData.dynamicInfo.pDynamicStates = mPipelineStateData.dynamicStates.data();
				mPipelineStateData.dynamicInfo.dynamicStateCount = static_cast<uint32_t>(mPipelineStateData.dynamicStates.size());
			}

			// Create pipeline layout
			{
				std::vector<vk::DescriptorSetLayout> setLayouts(_desc.descriptorSetLayouts.size());
				std::transform(_desc.descriptorSetLayouts.begin(),
							   _desc.descriptorSetLayouts.end(),
							   setLayouts.begin(),
							   [](const std::shared_ptr<DescriptorSetLayout>& _d) {return _d->get(); });

				vk::PipelineLayoutCreateInfo createInfo;
				createInfo.pPushConstantRanges = _desc.pushConstant.data();
				createInfo.pushConstantRangeCount = static_cast<uint32_t>(_desc.pushConstant.size());
				createInfo.pSetLayouts = setLayouts.data();
				createInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());

				mPipelineStateData.pipelineLayout = mDevice->get().createPipelineLayout(createInfo);
			}

			mPipelineStateData.pipelineCreateInfo.stageCount = usedStageCount;
			mPipelineStateData.pipelineCreateInfo.pStages = mPipelineStateData.shaderStageInfo;
			mPipelineStateData.pipelineCreateInfo.pVertexInputState = nullptr; // Set at runtime
			mPipelineStateData.pipelineCreateInfo.pInputAssemblyState = &mPipelineStateData.inputAssemblyInfo;
			mPipelineStateData.pipelineCreateInfo.pTessellationState = &mPipelineStateData.tesselationInfo;
			mPipelineStateData.pipelineCreateInfo.pViewportState = &mPipelineStateData.viewportInfo;
			mPipelineStateData.pipelineCreateInfo.pRasterizationState = &mPipelineStateData.rasterizationInfo;
			mPipelineStateData.pipelineCreateInfo.pMultisampleState = &mPipelineStateData.multisampleInfo;
			mPipelineStateData.pipelineCreateInfo.pDepthStencilState = &mPipelineStateData.depthStencilInfo;
			mPipelineStateData.pipelineCreateInfo.pColorBlendState = &mPipelineStateData.colorBlendInfo;
			mPipelineStateData.pipelineCreateInfo.pDynamicState = &mPipelineStateData.dynamicInfo;
			mPipelineStateData.pipelineCreateInfo.renderPass = nullptr;
			mPipelineStateData.pipelineCreateInfo.layout = mPipelineStateData.pipelineLayout;
			mPipelineStateData.pipelineCreateInfo.subpass = 0; // Set at runtime


		}

		GraphicsPipelineState::~GraphicsPipelineState() {
			if (mPipelineStateData.pipelineLayout) {
				mDevice->get().destroyPipelineLayout(mPipelineStateData.pipelineLayout);
			}
		}

		std::shared_ptr<Pipeline> GraphicsPipelineState::getPipeline(const std::shared_ptr<RenderPass>& _renderPass,
																	 uint32_t _subpassIndex,
																	 const std::shared_ptr<VertexInput>& _vertexInput,
																	 MeshTopology _drawMode) {
			// todo Find a nother way to generate the id of RenderPass
			auto renderPassKey = static_cast<uint32_t>(reinterpret_cast<intptr_t>(_renderPass.get()));

			PipelineKey key{ renderPassKey,_subpassIndex,_drawMode,_vertexInput->getId() };

			// A suitable graphice pipeline exists
			auto it = mPipelineMap.find(key);
			if (it != mPipelineMap.end())
				return it->second;

			// No suitable graphice pipeline
			// Create a new one
			auto newPipeline = createPipeline(_renderPass, _subpassIndex, _drawMode, _vertexInput);
			mPipelineMap[key] = newPipeline;
			return newPipeline;
		}

		GraphicsPipelineState::PipelineStateData::PipelineStateData() {
			inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo{
				{},
				vk::PrimitiveTopology::eTriangleList, //PrimitiveTopology
				false //PrimitiveRestartEnable
			};

			rasterizationInfo = vk::PipelineRasterizationStateCreateInfo{
				{},
				false, // DepathClampEnable
				false, // RasterizerDiscardEnable
				vk::PolygonMode::eFill, // PolygonMode
				vk::CullModeFlagBits::eNone, //CullMode
				vk::FrontFace::eCounterClockwise, //FrontFace
				false, // DepthBiasEnable
				0.0f, // DepthBiasConstantFactor
				0.0f, // DepthBiasClamp
				0.0f, // DepthBiasSlopeFactor
				1.0f  // LineWidth
			};


		}

		std::shared_ptr<Pipeline> GraphicsPipelineState::createPipeline(const std::shared_ptr<RenderPass>& _renderPass,
																		uint32_t _subpassIndex,
																		MeshTopology _drawMode,
																		const std::shared_ptr<VertexInput>& _vertexInput) {
			// todo Add more other common options
			mPipelineStateData.inputAssemblyInfo.topology = VulkanUtils::GetTopology(_drawMode);
			mPipelineStateData.pipelineCreateInfo.renderPass = _renderPass->get();
			mPipelineStateData.pipelineCreateInfo.subpass = _subpassIndex;
			mPipelineStateData.pipelineCreateInfo.pVertexInputState = &_vertexInput->getVertexInputStateInfo();

			vk::Pipeline pipeline = mDevice->get().createGraphicsPipeline(nullptr, mPipelineStateData.pipelineCreateInfo);

			return std::shared_ptr<Pipeline>(new Pipeline(_renderPass, mPipelineStateData.descriptorSetLayouts, _subpassIndex, pipeline, mPipelineStateData.pipelineLayout));
		}

		size_t GraphicsPipelineState::PipelineKey::Hash(const PipelineKey& _v) {
			size_t hash = 0;
			Utils::HashCombine(hash, _v.renderPassId);
			Utils::HashCombine(hash, _v.subpass);
			Utils::HashCombine(hash, _v.drawMode);
			Utils::HashCombine(hash, _v.vertexInputId);
			return hash;
		}

		bool GraphicsPipelineState::PipelineKey::Hasher::operator()(const PipelineKey& _key) const {
			return Hash(_key);
		}

		bool GraphicsPipelineState::PipelineKey::operator==(const PipelineKey& _other) const {
			return renderPassId == _other.renderPassId && subpass == _other.subpass && drawMode == _other.drawMode && vertexInputId == _other.vertexInputId;
		}

		bool GraphicsPipelineState::PipelineKey::operator!=(const PipelineKey& _other) const {
			return !(*this == _other);
		}
	}
}

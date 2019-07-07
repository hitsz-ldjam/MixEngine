#include "MxVkPipeline.h"
#include <utility>
#include "../Core/MxVkExcept.hpp"
#include "MxVkShader.h"

namespace Mix {
	namespace Graphics {
		//void Pipeline::clear() {
		//	mPipelineStates.reset();
		//}

		//void Pipeline::addShader(const Shader& _shader, const vk::SpecializationInfo* _specInfo) const {
		//	static char pName[] = "main";
		//	vk::PipelineShaderStageCreateInfo createInfo = {};
		//	createInfo.stage = _shader.stage();
		//	createInfo.module = _shader.get();
		//	createInfo.pName = pName;
		//	createInfo.pSpecializationInfo = _specInfo;

		//	mPipelineStates->shaders.push_back(std::move(createInfo));
		//}

		//void Pipeline::setVertexInput(const std::vector<vk::VertexInputBindingDescription>& _bindingDescri, const std::vector<vk::VertexInputAttributeDescription>& _attriDescri) const {
		//	mPipelineStates->vertexInput.pVertexBindingDescriptions = _bindingDescri.data();
		//	mPipelineStates->vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(_bindingDescri.size());
		//	mPipelineStates->vertexInput.pVertexAttributeDescriptions = _attriDescri.data();
		//	mPipelineStates->vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attriDescri.size());
		//}

		//void Pipeline::setInputAssembly(const vk::PrimitiveTopology _topology, const bool _primitiveRestart) const {
		//	mPipelineStates->inputAssembly.topology = _topology;
		//	mPipelineStates->inputAssembly.primitiveRestartEnable = _primitiveRestart;
		//}

		//void Pipeline::addViewport(std::initializer_list<vk::Viewport> _viewports) const {
		//	mPipelineStates->viewports.insert(mPipelineStates->viewports.end(), _viewports.begin(), _viewports.end());
		//}

		//void Pipeline::addViewport(const vk::Viewport& _viewport) const {
		//	mPipelineStates->viewports.push_back(_viewport);
		//}

		//void Pipeline::addScissor(std::initializer_list<vk::Rect2D> _scissors) const {
		//	mPipelineStates->scissors.insert(mPipelineStates->scissors.end(), _scissors.begin(), _scissors.end());
		//}

		//void Pipeline::addScissor(const vk::Rect2D & _scissors) const {
		//	mPipelineStates->scissors.push_back(_scissors);
		//}

		//void Pipeline::setRasterization(const vk::PolygonMode _polygonMode, const vk::CullModeFlags _cullMode, const vk::FrontFace _frontFace, const float _lineWidth, const bool _depthClampEnable, const bool _rasterizerDiscardEnable) const {

		//	mPipelineStates->rasterization.polygonMode = _polygonMode;
		//	mPipelineStates->rasterization.cullMode = _cullMode;
		//	mPipelineStates->rasterization.frontFace = _frontFace;
		//	mPipelineStates->rasterization.lineWidth = _lineWidth;
		//	mPipelineStates->rasterization.depthClampEnable = _depthClampEnable;
		//	mPipelineStates->rasterization.rasterizerDiscardEnable = _rasterizerDiscardEnable;
		//}

		//void Pipeline::setDepthBias(const bool _enable, const float _constantFactor, const float _slopeFactor, const float _clamp) const {
		//	mPipelineStates->rasterization.depthBiasEnable = _enable;
		//	mPipelineStates->rasterization.depthBiasConstantFactor = _constantFactor;
		//	mPipelineStates->rasterization.depthBiasSlopeFactor = _slopeFactor;
		//	mPipelineStates->rasterization.depthBiasClamp = _clamp;
		//}

		//void Pipeline::setMultiSample(const vk::SampleCountFlagBits _samples, const bool _sampleShading, const float _minSampleShading, const vk::SampleMask * _sampleMask, const bool _alphaToCoverageEnable, const bool _alphaToOneEnable) const {

		//	mPipelineStates->multisample.sampleShadingEnable = _sampleShading;
		//	mPipelineStates->multisample.rasterizationSamples = _samples;
		//	mPipelineStates->multisample.minSampleShading = _minSampleShading;
		//	mPipelineStates->multisample.pSampleMask = _sampleMask;
		//	mPipelineStates->multisample.alphaToCoverageEnable = _alphaToCoverageEnable;
		//	mPipelineStates->multisample.alphaToOneEnable = _alphaToOneEnable;
		//}

		//void Pipeline::setDepthTest(const bool _depthTestEnable, const bool _depthWriteEnable, const vk::CompareOp _depthCompareOp) const {
		//	mPipelineStates->depthStencil.depthTestEnable = _depthTestEnable;
		//	mPipelineStates->depthStencil.depthWriteEnable = _depthWriteEnable;
		//	mPipelineStates->depthStencil.depthCompareOp = _depthCompareOp;
		//}

		//void Pipeline::setDepthBoundsTest(const bool _enable, const float _minBounds, const float _maxBounds) const {
		//	mPipelineStates->depthStencil.depthBoundsTestEnable = _enable;
		//	mPipelineStates->depthStencil.minDepthBounds = _minBounds;
		//	mPipelineStates->depthStencil.maxDepthBounds = _maxBounds;
		//}

		//void Pipeline::setStencilTest(const bool _enable, const vk::StencilOpState & _front, const vk::StencilOpState & _back) const {
		//	mPipelineStates->depthStencil.stencilTestEnable = _enable;
		//	mPipelineStates->depthStencil.front = _front;
		//	mPipelineStates->depthStencil.back = _back;
		//}

		//void Pipeline::addDefaultBlendAttachment() const {
		//	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		//	colorBlendAttachment.blendEnable = true; //enable blend
		//	colorBlendAttachment.colorWriteMask =
		//		vk::ColorComponentFlagBits::eR |
		//		vk::ColorComponentFlagBits::eG |
		//		vk::ColorComponentFlagBits::eB |
		//		vk::ColorComponentFlagBits::eA;
		//	//finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		//	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
		//	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
		//	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		//	//finalColor.a   = (srcAlphaBlendFactor * newColor.a)   <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		//	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		//	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		//	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
		//	addBlendAttachments(colorBlendAttachment);
		//}

		//void Pipeline::addBlendAttachments(const vk::PipelineColorBlendAttachmentState & _attachment) const {
		//	mPipelineStates->colorBlendAttachments.push_back(_attachment);
		//}

		//void Pipeline::addBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& _attachments) const {
		//	mPipelineStates->colorBlendAttachments.insert(mPipelineStates->colorBlendAttachments.end(),
		//												  _attachments.begin(),
		//												  _attachments.end());
		//}

		//void Pipeline::setBlend(const bool _logicalOpEnable, const vk::LogicOp _logicOp, const float _constantR, const float _constantG, const float _constantB, const float _constantA) const {
		//	mPipelineStates->colorBlend.logicOpEnable = _logicalOpEnable;
		//	mPipelineStates->colorBlend.logicOp = _logicOp;
		//	mPipelineStates->colorBlend.blendConstants[0] = _constantR;
		//	mPipelineStates->colorBlend.blendConstants[1] = _constantG;
		//	mPipelineStates->colorBlend.blendConstants[2] = _constantB;
		//	mPipelineStates->colorBlend.blendConstants[3] = _constantA;
		//}

		//void Pipeline::addDynamicState(const vk::DynamicState _dynamicState) const {
		//	mPipelineStates->dynamicStates.push_back(_dynamicState);
		//}

		//void Pipeline::addDynamicState(const std::vector<vk::DynamicState>& _dynamicStates) const {
		//	mPipelineStates->dynamicStates.insert(mPipelineStates->dynamicStates.end(),
		//										  _dynamicStates.begin(),
		//										  _dynamicStates.end());
		//}

		//void Pipeline::addDescriptorSetLayout(const vk::DescriptorSetLayout _setLayout) const {
		//	mPipelineStates->descriptorSetLayouts.push_back(_setLayout);
		//}

		//void Pipeline::addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& _setLayouts) const {
		//	mPipelineStates->descriptorSetLayouts.insert(mPipelineStates->descriptorSetLayouts.end(),
		//												 _setLayouts.begin(),
		//												 _setLayouts.end());
		//}

		//void Pipeline::addPushConstantRange(vk::ShaderStageFlags _stageFlags, uint32_t _offset, uint32_t _size) const {
		//	mPipelineStates->pushConstantRanges.push_back(vk::PushConstantRange(_stageFlags, _offset, _size));
		//}

		//void Pipeline::addPushConstantRange(const vk::PushConstantRange & _range) const {
		//	mPipelineStates->pushConstantRanges.push_back(_range);
		//}

		//void Pipeline::addPushConstantRanges(const std::vector<vk::PushConstantRange>& _ranges) const {
		//	mPipelineStates->pushConstantRanges.insert(mPipelineStates->pushConstantRanges.end(), _ranges.begin(), _ranges.end());
		//}

		//bool Pipeline::create() {
		//	// create pipeline layout
		//	vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
		//	layoutCreateInfo.pSetLayouts = mPipelineStates->descriptorSetLayouts.data();
		//	layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(mPipelineStates->descriptorSetLayouts.size());
		//	layoutCreateInfo.pPushConstantRanges = mPipelineStates->pushConstantRanges.data();
		//	layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(mPipelineStates->pushConstantRanges.size());

		//	mPipelineLayout = mRenderPass->getDevice()->get().createPipelineLayoutUnique(layoutCreateInfo);

		//	vk::PipelineViewportStateCreateInfo viewportState = {};
		//	viewportState.pViewports = mPipelineStates->viewports.data();
		//	viewportState.viewportCount = static_cast<uint32_t>(mPipelineStates->viewports.size());
		//	viewportState.pScissors = mPipelineStates->scissors.data();
		//	viewportState.scissorCount = static_cast<uint32_t>(mPipelineStates->scissors.size());

		//	if (mPipelineStates->colorBlendAttachments.empty())
		//		addDefaultBlendAttachment();

		//	mPipelineStates->colorBlend.pAttachments = mPipelineStates->colorBlendAttachments.data();
		//	mPipelineStates->colorBlend.attachmentCount = static_cast<uint32_t>(mPipelineStates->colorBlendAttachments.size());

		//	vk::PipelineDynamicStateCreateInfo dynamicState = {};
		//	dynamicState.pDynamicStates = mPipelineStates->dynamicStates.data();
		//	dynamicState.dynamicStateCount = static_cast<uint32_t>(mPipelineStates->dynamicStates.size());

		//	vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
		//	pipelineCreateInfo.pStages = mPipelineStates->shaders.data();
		//	pipelineCreateInfo.stageCount = static_cast<uint32_t>(mPipelineStates->shaders.size());
		//	pipelineCreateInfo.pVertexInputState = &mPipelineStates->vertexInput;
		//	pipelineCreateInfo.pInputAssemblyState = &mPipelineStates->inputAssembly;
		//	pipelineCreateInfo.pViewportState = &viewportState;
		//	pipelineCreateInfo.pRasterizationState = &mPipelineStates->rasterization;
		//	pipelineCreateInfo.pMultisampleState = &mPipelineStates->multisample;
		//	pipelineCreateInfo.pDepthStencilState = &mPipelineStates->depthStencil;
		//	pipelineCreateInfo.pColorBlendState = &mPipelineStates->colorBlend;

		//	if (dynamicState.dynamicStateCount == 0)
		//		pipelineCreateInfo.pDynamicState = nullptr;
		//	else
		//		pipelineCreateInfo.pDynamicState = &dynamicState;

		//	pipelineCreateInfo.layout = *mPipelineLayout;
		//	pipelineCreateInfo.renderPass = mRenderPass->get();
		//	pipelineCreateInfo.subpass = mSubpassIndex;
		//	pipelineCreateInfo.basePipelineHandle = nullptr;	//基础管线（vulkan允许在已经存在的管线上派生新的管线）
		//	pipelineCreateInfo.basePipelineIndex = -1;

		//	mPipeline = mRenderPass->getDevice()->get().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);

		//	clear();
		//	return true;
		//}

		void Pipeline::swap(Pipeline& _other) noexcept {
			mRenderPass.swap(_other.mRenderPass);
			std::swap(mPipeline, _other.mPipeline);
			std::swap(mPipelineLayout, _other.mPipelineLayout);
			std::swap(mSubpassIndex, _other.mSubpassIndex);
		}

		Pipeline::~Pipeline() {
			if(mPipeline) {
				mRenderPass->getDevice()->get().destroyPipeline(mPipeline);
				mRenderPass->getDevice()->get().destroyPipelineLayout(mPipelineLayout);
			}
		}

		Pipeline::Pipeline(std::shared_ptr<RenderPass> _renderPass,
						   ArrayProxy<std::shared_ptr<DescriptorSetLayout>> _descriptorSetLayout,
						   const uint32_t _subpassIndex,
						   const vk::Pipeline& _pipeline,
						   const vk::PipelineLayout& _layout)
			: mRenderPass(std::move(_renderPass)),
			mDescriptorSetLayout(_descriptorSetLayout.begin(), _descriptorSetLayout.end()),
			mPipeline(_pipeline),
			mPipelineLayout(_layout),
			mSubpassIndex(_subpassIndex) {
		}

		/*std::shared_ptr<Pipeline> PipelineMgr::createPipeline(const std::string & _name,
															  const std::shared_ptr<RenderPass>& _renderPass,
															  const uint32_t _subpassIndex) {
			if (mPipelines.count(_name) != 0)
				throw PipelineAlreadyExist(_name);

			auto pipeline = std::make_shared<Pipeline>(_renderPass, _subpassIndex);
			mPipelines[_name] = pipeline;
			return pipeline;
		}

		std::shared_ptr<Pipeline> PipelineMgr::getPipeline(const std::string & _name) {
			if (mPipelines.count(_name) == 0)
				throw PipelineNotFound(_name);

			return mPipelines[_name];
		}

		void PipelineMgr::destroyPipeline(const std::string & _name) {
			if (mPipelines.count(_name) == 0)
				throw PipelineNotFound(_name);

			mPipelines.erase(_name);
		}

		void PipelineMgr::destroy() {
			mPipelines.clear();
		}*/
	}
}

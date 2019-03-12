#include "MxVkPipeline.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Pipeline, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Pipeline);

        void Pipeline::clear() {
            if (mPipelineStates) {
                delete mPipelineStates;
                mPipelineStates = nullptr;
            }
        }

        Pipeline & Pipeline::operator=(Pipeline && pipeline) {
            this->mCore = pipeline.mCore;
            this->mPipeline = pipeline.mPipeline;
            this->mPipelineLayout = pipeline.mPipelineLayout;
            this->mRenderPass = pipeline.mRenderPass;
            this->mSubpassIndex = pipeline.mSubpassIndex;
            this->mPipelineStates = pipeline.mPipelineStates;

            pipeline.mCore = nullptr;
            pipeline.mPipelineStates = nullptr;
            pipeline.mPipeline = nullptr;
            pipeline.mPipelineLayout = nullptr;
            pipeline.mRenderPass = nullptr;
            pipeline.mSubpassIndex = 0;
            return *this;
        }

        void Pipeline::init(const Core * core) {
            mCore = core;
            mPipelineStates = new PipelineStates;
        }

        void Pipeline::setTargetRenderPass(const vk::RenderPass renderPass, const uint32_t subpassIndex) {
            mRenderPass = renderPass;
            mSubpassIndex = subpassIndex;
        }

        void Pipeline::addShader(const vk::ShaderStageFlagBits stage, const vk::ShaderModule shader, const vk::SpecializationInfo* specInfo) {
            static char pName[] = "main";
            vk::PipelineShaderStageCreateInfo createInfo = {};
            createInfo.stage = stage;
            createInfo.module = shader;
            createInfo.pName = pName;
            createInfo.pSpecializationInfo = specInfo;

            mPipelineStates->shaders.push_back(std::move(createInfo));
        }

        void Pipeline::setVertexInput(const std::vector<vk::VertexInputBindingDescription>& bindingDescri, const std::vector<vk::VertexInputAttributeDescription>& attriDescri) {
            mPipelineStates->vertexInput.pVertexBindingDescriptions = bindingDescri.data();
            mPipelineStates->vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescri.size());
            mPipelineStates->vertexInput.pVertexAttributeDescriptions = attriDescri.data();
            mPipelineStates->vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attriDescri.size());
        }

        void Pipeline::setInputAssembly(const vk::PrimitiveTopology topology, const bool primitiveRestart) {
            mPipelineStates->inputAssembly.topology = topology;
            mPipelineStates->inputAssembly.primitiveRestartEnable = primitiveRestart;
        }

        void Pipeline::addViewport(const std::vector<vk::Viewport>& viewports) {
            mPipelineStates->viewports.insert(mPipelineStates->viewports.end(), viewports.begin(), viewports.end());
        }

        void Pipeline::addViewport(const vk::Viewport& viewport) {
            mPipelineStates->viewports.push_back(viewport);
        }

        void Pipeline::addScissor(const std::vector<vk::Rect2D>& scissors) {
            mPipelineStates->scissors.insert(mPipelineStates->scissors.end(), scissors.begin(), scissors.end());
        }

        void Pipeline::addScissor(const vk::Rect2D & scissors) {
            mPipelineStates->scissors.push_back(scissors);
        }

        void Pipeline::setRasterization(const vk::PolygonMode polygonMode, const vk::CullModeFlags cullMode, const vk::FrontFace frontFace, const float lineWidth, const bool depthClampEnable, const bool rasterizerDiscardEnable) {

            mPipelineStates->rasterization.polygonMode = polygonMode;
            mPipelineStates->rasterization.cullMode = cullMode;
            mPipelineStates->rasterization.frontFace = frontFace;
            mPipelineStates->rasterization.lineWidth = lineWidth;
            mPipelineStates->rasterization.depthClampEnable = depthClampEnable;
            mPipelineStates->rasterization.rasterizerDiscardEnable = rasterizerDiscardEnable;
        }

        void Pipeline::setDepthBias(const bool enable, const float constantFactor, const float slopeFactor, const float clamp) {
            mPipelineStates->rasterization.depthBiasEnable = enable;
            mPipelineStates->rasterization.depthBiasConstantFactor = constantFactor;
            mPipelineStates->rasterization.depthBiasSlopeFactor = slopeFactor;
            mPipelineStates->rasterization.depthBiasClamp = clamp;
        }

        void Pipeline::setMultiSample(const vk::SampleCountFlagBits samples, const bool sampleShading, const float minSampleShading, const vk::SampleMask * sampleMask, const bool alphaToCoverageEnable, const bool alphaToOneEnable) {

            mPipelineStates->multisample.sampleShadingEnable = sampleShading;
            mPipelineStates->multisample.rasterizationSamples = samples;
            mPipelineStates->multisample.minSampleShading = minSampleShading;
            mPipelineStates->multisample.pSampleMask = sampleMask;
            mPipelineStates->multisample.alphaToCoverageEnable = alphaToCoverageEnable;
            mPipelineStates->multisample.alphaToOneEnable = alphaToOneEnable;
        }

        void Pipeline::setDepthTest(const bool depthTestEnable, const bool depthWriteEnable, const vk::CompareOp depthCompareOp) {
            mPipelineStates->depthStencil.depthTestEnable = depthTestEnable;
            mPipelineStates->depthStencil.depthWriteEnable = depthWriteEnable;
            mPipelineStates->depthStencil.depthCompareOp = depthCompareOp;
        }

        void Pipeline::setDepthBoundsTest(const bool enable, const float minBounds, const float maxBounds) {
            mPipelineStates->depthStencil.depthBoundsTestEnable = enable;
            mPipelineStates->depthStencil.minDepthBounds = minBounds;
            mPipelineStates->depthStencil.maxDepthBounds = maxBounds;
        }

        void Pipeline::setStencilTest(const bool enable, const vk::StencilOpState & front, const vk::StencilOpState & back) {
            mPipelineStates->depthStencil.stencilTestEnable = enable;
            mPipelineStates->depthStencil.front = front;
            mPipelineStates->depthStencil.back = back;
        }

        void Pipeline::addDefaultBlendAttachments() {
            vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.blendEnable = true; //enable blend
            colorBlendAttachment.colorWriteMask =
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA;
            //finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
            colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
            //finalColor.a   = (srcAlphaBlendFactor * newColor.a)   <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
            colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
            colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
            addBlendAttachments(std::move(colorBlendAttachment));
        }

        void Pipeline::addBlendAttachments(const vk::PipelineColorBlendAttachmentState & attachment) {
            mPipelineStates->colorBlendAttachments.push_back(attachment);
        }

        void Pipeline::addBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState>& attachments) {
            mPipelineStates->colorBlendAttachments.insert(mPipelineStates->colorBlendAttachments.end(),
                                                          attachments.begin(),
                                                          attachments.end());
        }

        void Pipeline::setBlend(const bool logicalOpEnable, const vk::LogicOp logicOp, const float constantR, const float constantG, const float constantB, const float constantA) {
            mPipelineStates->colorBlend.logicOpEnable = logicalOpEnable;
            mPipelineStates->colorBlend.logicOp = logicOp;
            mPipelineStates->colorBlend.pAttachments = mPipelineStates->colorBlendAttachments.data();
            mPipelineStates->colorBlend.attachmentCount = static_cast<uint32_t>(mPipelineStates->colorBlendAttachments.size());
            mPipelineStates->colorBlend.blendConstants[0] = constantR;
            mPipelineStates->colorBlend.blendConstants[1] = constantG;
            mPipelineStates->colorBlend.blendConstants[2] = constantB;
            mPipelineStates->colorBlend.blendConstants[3] = constantA;
        }

        void Pipeline::addDynamicState(const vk::DynamicState dynamicState) {
            mPipelineStates->dynamicStates.push_back(dynamicState);
        }

        void Pipeline::addDynamicState(const std::vector<vk::DynamicState>& dynamicStates) {
            mPipelineStates->dynamicStates.insert(mPipelineStates->dynamicStates.end(),
                                                  dynamicStates.begin(),
                                                  dynamicStates.end());
        }

        void Pipeline::addDescriptorSetLayout(const vk::DescriptorSetLayout setLayout) {
            mPipelineStates->descriptorSetLayouts.push_back(setLayout);
        }

        void Pipeline::addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayout>& setLayouts) {
            mPipelineStates->descriptorSetLayouts.insert(mPipelineStates->descriptorSetLayouts.end(),
                                                         setLayouts.begin(),
                                                         setLayouts.end());
        }

        void Pipeline::addPushConstantRange(vk::ShaderStageFlags stageFlags, uint32_t offset, uint32_t size) {
            mPipelineStates->pushConstantRanges.push_back(vk::PushConstantRange(stageFlags, offset, size));
        }

        void Pipeline::addPushConstantRange(const vk::PushConstantRange & range) {
            mPipelineStates->pushConstantRanges.push_back(range);
        }

        void Pipeline::addPushConstantRanges(const std::vector<vk::PushConstantRange>& ranges) {
            mPipelineStates->pushConstantRanges.insert(mPipelineStates->pushConstantRanges.end(), ranges.begin(), ranges.end());
        }

        bool Pipeline::create() {
            // create pipeline layout
            vk::PipelineLayoutCreateInfo layoutCreateInfo = {};
            layoutCreateInfo.pSetLayouts = mPipelineStates->descriptorSetLayouts.data();
            layoutCreateInfo.setLayoutCount = static_cast<uint32_t>(mPipelineStates->descriptorSetLayouts.size());
            layoutCreateInfo.pPushConstantRanges = mPipelineStates->pushConstantRanges.data();
            layoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(mPipelineStates->pushConstantRanges.size());

            mPipelineLayout = mCore->device().createPipelineLayout(layoutCreateInfo);

            vk::PipelineViewportStateCreateInfo viewportState = {};
            viewportState.pViewports = mPipelineStates->viewports.data();
            viewportState.viewportCount = static_cast<uint32_t>(mPipelineStates->viewports.size());
            viewportState.pScissors = mPipelineStates->scissors.data();
            viewportState.scissorCount = static_cast<uint32_t>(mPipelineStates->scissors.size());

            mPipelineStates->colorBlend.pAttachments = mPipelineStates->colorBlendAttachments.data();
            mPipelineStates->colorBlend.attachmentCount = static_cast<uint32_t>(mPipelineStates->colorBlendAttachments.size());

            vk::PipelineDynamicStateCreateInfo dynamicState = {};
            dynamicState.pDynamicStates = mPipelineStates->dynamicStates.data();
            dynamicState.dynamicStateCount = static_cast<uint32_t>(mPipelineStates->dynamicStates.size());

            vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
            pipelineCreateInfo.pStages = mPipelineStates->shaders.data();
            pipelineCreateInfo.stageCount = static_cast<uint32_t>(mPipelineStates->shaders.size());
            pipelineCreateInfo.pVertexInputState = &mPipelineStates->vertexInput;
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

            pipelineCreateInfo.layout = mPipelineLayout;
            pipelineCreateInfo.renderPass = mRenderPass;
            pipelineCreateInfo.subpass = mSubpassIndex;
            pipelineCreateInfo.basePipelineHandle = nullptr;	//基础管线（vulkan允许在已经存在的管线上派生新的管线）
            pipelineCreateInfo.basePipelineIndex = -1;

            mPipeline = mCore->device().createGraphicsPipeline(nullptr, pipelineCreateInfo);

            clear();
            return true;
        }

        void Pipeline::destroy() {
            if (!mCore)
                return;

            mCore->device().destroyPipeline(mPipeline);
            mCore->device().destroyPipelineLayout(mPipelineLayout);
            mCore = nullptr;
            mPipeline = nullptr;
            mPipelineLayout = nullptr;
        }

        MX_IMPLEMENT_RTTI_NoCreateFunc(PipelineMgr, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(PipelineMgr);

        Pipeline & PipelineMgr::createPipeline(const std::string & name, const vk::RenderPass renderPass, const uint32_t subpassIndex) {
            if (mPipelines.count(name) != 0)
                throw PipelineAlreadyExist(name);

            Pipeline pipeline;
            pipeline.init(mCore);
            pipeline.setTargetRenderPass(renderPass, subpassIndex);
            mPipelines[name] = std::move(pipeline);
            return mPipelines[name];
        }
        const Pipeline & PipelineMgr::getPipeline(const std::string & name) {
            if (mPipelines.count(name) == 0)
                throw PipelineNotFound(name);

            return mPipelines[name];
        }
        void PipelineMgr::destroyPipeline(const std::string & name) {
            if (mPipelines.count(name) == 0)
                throw PipelineNotFound(name);

            mPipelines.erase(name);
        }
        void PipelineMgr::destroy() {
            mPipelines.clear();
            mCore = nullptr;
        }
    }
}
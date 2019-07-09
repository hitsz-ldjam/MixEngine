#include "MxVkPipeline.h"
#include "MxVkShader.h"
#include "../Descriptor/MxVkDescriptorSet.h"
#include <utility>


namespace Mix {
	namespace Graphics {
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
						   ArrayProxy<DescriptorSetLayout> _descriptorSetLayout,
						   const uint32_t _subpassIndex,
						   const vk::Pipeline& _pipeline,
						   const vk::PipelineLayout& _layout)
			: mRenderPass(std::move(_renderPass)),
			mDescriptorSetLayout(_descriptorSetLayout.begin(), _descriptorSetLayout.end()),
			mPipeline(_pipeline),
			mPipelineLayout(_layout),
			mSubpassIndex(_subpassIndex) {
		}
	}
}

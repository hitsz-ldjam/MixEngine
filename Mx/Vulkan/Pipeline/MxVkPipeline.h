#pragma once
#ifndef MX_VK_PIPELINE_H_
#define MX_VK_PIPELINE_H_

#include "MxVkRenderPass.h"
#include "../../Utils/MxArrayProxy.h"

namespace Mix {
	namespace Vulkan {
		class ShaderModule;
		class DescriptorSetLayout;

		class Pipeline : public GeneralBase::NoCopyBase {
			friend class PipelineFactory;
		public:
			Pipeline(Pipeline&& _other) noexcept { swap(_other); }

			Pipeline& operator=(Pipeline&& _other) noexcept { swap(_other); return *this; }

			void swap(Pipeline& _other) noexcept;

			~Pipeline();

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

		};
	}
}
#endif // !MX_VK_PIPELINE_H_

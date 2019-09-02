#pragma once
#ifndef MX_VK_STANDARD_RENDERER_H_
#define MX_VK_STANDARD_RENDERER_H_

#include "MxVkShaderBase.h"
#include "../Buffers/MxVkUniformBuffer.h"
#include "../FrameBuffer/MxVkFramebuffer.h"
#include <deque>
#include "../Pipeline/MxVkGraphicsPipelineState.h"

namespace Mix {
	class Ui;

	namespace Vulkan {
		class Pipeline;
		class RenderPass;
		class Image;
		class Swapchain;
		class DescriptorSetLayout;
		class DescriptorPool;

		class StandardShader final : public ShaderBase {
		public:
			explicit StandardShader(VulkanAPI* _vulkan);

			~StandardShader() override;

			void render(Shader& _shader, ArrayProxy<const RenderElement> _renderElements, const Camera& _camera) override;

			uint32_t newMaterial() override;

			void deleteMaterial(uint32_t _id) override;

		private:
			std::shared_ptr<Device> mDevice;
			std::shared_ptr<Swapchain> mSwapchain;

			std::shared_ptr<GraphicsPipelineState> mGraphicsPipelineState;
			std::shared_ptr<RenderPass> mRenderPass;

			std::vector<FrameBuffer> mFrameBuffers;

			std::shared_ptr<Image> mDepthStencil;
			vk::ImageView mDepthStencilView;

			std::shared_ptr<DescriptorSetLayout> mStaticParamDescriptorSetLayout;
			std::shared_ptr<DescriptorSetLayout> mDynamicPamramDescriptorSetLayout;
			std::shared_ptr<DescriptorPool> mDescriptorPool;
			std::vector<DescriptorSet> mStaticDescriptorSets;
			std::vector<Buffer> mCameraUniforms;
			std::vector<DynamicUniformBuffer> mDynamicUniform;

			uint32_t mDefaultMaterialCount = 10;
			std::unordered_map<std::string, uint32_t> mMaterialNameBindingMap;
			std::vector<std::vector<DescriptorSet>> mMaterialDescs;
			std::deque<uint32_t> mUnusedId;

			// test GUI
			/*Ui* mUi = nullptr;
			vk::DescriptorSet mGuiDescriptorSet;

			std::shared_ptr<Pipeline> mGuiPipeline;*/

			void buildRenderPass();
			void buildFrameBuffer();
			void buildDescriptorSetLayout();
			void buildPipeline();
			void buildDescriptorSet();
			void buildPropertyBlock();

			void updateShaderParam(uint32_t _currFrame, Shader& _shader);
			void updateMaterailParam(uint32_t _currFrame, Material& _material);
		};
	}
}

#endif

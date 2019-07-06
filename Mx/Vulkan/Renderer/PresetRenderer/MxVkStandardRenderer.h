#pragma once
#ifndef MX_VK_STANDARD_RENDERER_H_
#define MX_VK_STANDARD_RENDERER_H_

#include "../MxVkRendererBase.h"
#include "../../Buffers/MxVkUniformBuffer.h"
#include "../../FrameBuffer/MxVkFramebuffer.h"

namespace Mix {
	class Ui;

	namespace Graphics {
		class Pipeline;
		class RenderPass;
		class Image;
		class Swapchain;

		class StandardRenderer : public RendererBase {
		public:
			~StandardRenderer() override;

			void init() override;

			void update(const uint32_t _currFrame) override;

			void render(CommandBufferHandle& _cmd, const uint32_t _currFrame) override;

		private:
			std::shared_ptr<Device> mDevice;
			std::shared_ptr<Swapchain> mSwapchain;

			std::vector<Buffer> mCameraUniforms;

			std::vector<DynamicUniformBuffer> mDynamicUniform;

			std::shared_ptr<Pipeline> mPipeline;
			std::shared_ptr<RenderPass> mRenderPass;

			std::map<std::string, std::shared_ptr<DescriptorSetLayout>> mDescriptorSetLayout;

			std::vector<FrameBuffer> mFrameBuffers;

			std::shared_ptr<Image> mDepthStencil;
			vk::ImageView mDepthStencilView;
			std::vector<vk::DescriptorSet> mDescriptorSets;

			// test GUI
			Ui* mUi = nullptr;
			std::shared_ptr<DescriptorSetLayout> mGuiDescriptorSetLayout;
			vk::DescriptorSet mGuiDescriptorSet;

			std::shared_ptr<Pipeline> mGuiPipeline;

			void buildDescriptor();
			void buildRenderPass();
			void buildFrameBuffer();
			void buildPipeline();
		};
	}
}

#endif

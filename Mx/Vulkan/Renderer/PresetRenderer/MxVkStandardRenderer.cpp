#include "MxVkStandardRenderer.h"
#include "../../MxVkGraphics.h"
#include "../../Buffers/MxVkUniform.h"
#include "../../Swapchain/MxVkSwapchain.h"
#include "../../Pipeline/MxVkRenderPass.h"
#include "../../../../MixEngine.h"
#include "../../Pipeline/MxVkPipeline.h"
#include "../../FrameBuffer/MxVkFramebuffer.h"
#include "../../../Component/MeshFilter/MxMeshFilter.h"
#include "../../../Resource/Shader/MxShaderSource.h"
#include "../../../GUI/MxUi.h"
#include "../../../Resource/Texture/MxTexture.h"
#include "../../../GUI/imgui/imgui.h"
#include "../../Pipeline/MxVkPipelineFactory.h"
#include "../../../GameObject/MxGameObject.h"
#include "../../../Preset/Camera/MxCamera.h"

namespace Mix {
	namespace Graphics {
		StandardRenderer::~StandardRenderer() {
			mDevice->get().destroyImageView(mDepthStencilView);
		}

		void StandardRenderer::init() {
			mDevice = mVulkan->getLogicalDevice();
			mSwapchain = mVulkan->getSwapchain();

			auto imageCount = mSwapchain->imageCount();
			mDynamicUniform.reserve(imageCount);
			// mTestDynamic.reserve(imageCount);
			mCameraUniforms.reserve(imageCount);

			for (size_t i = 0; i < imageCount; ++i) {
				mDynamicUniform.emplace_back(mVulkan->getAllocator(),
											 sizeof(Uniform::MeshUniform),
											 120);

				mCameraUniforms.emplace_back(mVulkan->getAllocator(),
											 vk::BufferUsageFlagBits::eUniformBuffer,
											 vk::MemoryPropertyFlagBits::eHostVisible |
											 vk::MemoryPropertyFlagBits::eHostCoherent,
											 sizeof(Uniform::CameraUniform));
			}

			buildRenderPass();
			buildFrameBuffer();
			buildPipeline();
			buildDescriptor();
		}

		void StandardRenderer::update(const uint32_t _currFrame) {
			auto camera = GameObject::Find("Camera");

			if (camera) {
				auto& transform = camera->transform();
				auto cb = camera->getComponent<Camera>();

				Uniform::CameraUniform ubo;

				ubo.position = transform.getPosition();

				ubo.viewMat = cb->getViewMat();

				ubo.projMat = cb->getProjMat();

				mCameraUniforms[_currFrame].uploadData(&ubo, sizeof(ubo));
			}
		}

		void StandardRenderer::render(CommandBufferHandle& _cmd, const uint32_t _currFrame) {
			std::vector<vk::ClearValue> clearValues(2);
			clearValues[0].color = std::array<float, 4>{0.0f, 0.75f, 1.0f, 1.0f};
			clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0);

			mRenderPass->beginRenderPass(_cmd.get(),
										 mFrameBuffers[_currFrame].get(),
										 clearValues,
										 mSwapchain->extent());

			_cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->get());

			auto filters = Object::FindObjectsOfType<MeshFilter>();

			for (auto filter : filters) {
				auto mesh = filter->getMesh();
				auto& trans = filter->getGameObject()->transform();

				Uniform::MeshUniform uniform;
				uniform.modelMat = trans.localToWorldMatrix();
				uniform.normMat = uniform.modelMat.transpose().inverse();

				mDynamicUniform[_currFrame].pushBack(&uniform, sizeof uniform);

				_cmd.get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
											  mPipeline->pipelineLayout(),
											  0,
											  mDescriptorSets[_currFrame],
											  mDynamicUniform[_currFrame].getOffset());

				mDynamicUniform[_currFrame].next();

				_cmd.get().bindVertexBuffers(0, mesh->getVertexBuffer()->get(), { 0 });
				_cmd.get().bindIndexBuffer(mesh->getIndexBUffer()->get(), 0, vk::IndexType::eUint32);

				for (auto& submesh : mesh->getSubmesh()) {
					_cmd.get().drawIndexed(submesh.indexCount,
										   1,
										   submesh.firstIndex,
										   submesh.firstVertex,
										   0);
				}

			}
			// Test Gui
			if (ImGui::GetDrawData()->CmdListsCount > 0) {
				_cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, mGuiPipeline->get());
				_cmd.get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
											  mGuiPipeline->pipelineLayout(),
											  0,
											  mGuiDescriptorSet,
											  nullptr);
				_cmd.get().bindVertexBuffers(0, mUi->getVertexBuffer().get(), { 0 });
				_cmd.get().bindIndexBuffer(mUi->getIndexBuffer().get(), 0, mUi->getIndexType());

				_cmd.get().pushConstants(mGuiPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0,
										 sizeof(Math::Vector2f), mUi->getScale().linear);
				_cmd.get().pushConstants(mGuiPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex,
										 sizeof(Math::Vector2f), sizeof(Math::Vector2f), mUi->getTranslate().linear);

				auto drawData = mUi->getDrawData();

				int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
				int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

				vk::Viewport viewport;
				viewport.x = 0;
				viewport.y = 0;
				viewport.width = static_cast<float>(fbWidth);
				viewport.height = static_cast<float>(fbHeight);
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				_cmd.get().setViewport(0, viewport);

				Math::Vector2f clipOff{ drawData->DisplayPos.x, drawData->DisplayPos.y };
				// (0,0) unless using multi-viewports
				Math::Vector2f clipScale{ drawData->FramebufferScale.x, drawData->FramebufferScale.y };
				// (1,1) unless using retina display which are often (2,2)

				// Render command lists
				// (Because we merged all buffers into a single one, we maintain our own offset into them)
				int vtxOffset = 0;
				int idxOffset = 0;
				for (int n = 0; n < drawData->CmdListsCount; n++) {
					const ImDrawList* cmdList = drawData->CmdLists[n];
					for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++) {
						const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
						// Project scissor/clipping rectangles into framebuffer space
						Math::Vector4f clipRect;
						clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
						clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
						clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
						clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

						if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f) {
							// Negative offsets are illegal for vkCmdSetScissor
							if (clipRect.x < 0.0f)
								clipRect.x = 0.0f;
							if (clipRect.y < 0.0f)
								clipRect.y = 0.0f;

							// Apply scissor/clipping rectangle
							vk::Rect2D scissor;
							scissor.offset.x = static_cast<int32_t>(clipRect.x);
							scissor.offset.y = static_cast<int32_t>(clipRect.y);
							scissor.extent.width = static_cast<uint32_t>(clipRect.z - clipRect.x);
							scissor.extent.height = static_cast<uint32_t>(clipRect.w - clipRect.y);
							_cmd.get().setScissor(0, 1, &scissor);
							// Draw
							_cmd.get().drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + idxOffset,
												   pcmd->VtxOffset + vtxOffset, 0);
						}
					}
					idxOffset += cmdList->IdxBuffer.Size;
					vtxOffset += cmdList->VtxBuffer.Size;
				}
			}


			mDynamicUniform[_currFrame].reset();
			mRenderPass->endRenderPass(_cmd.get());
		}

		void StandardRenderer::buildDescriptor() {
			mDescriptorSets = mVulkan->getDescriptorPool()->allocDescriptorSet(mPipeline->descriptorSetLayouts()[0].get(),
																			   mSwapchain->imageCount());

			// update descriptor sets
			for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
				std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};
				auto write0 = mCameraUniforms[i].getWriteDescriptor(0, vk::DescriptorType::eUniformBuffer);
				auto write1 = mDynamicUniform[i].getWriteDescriptorSet(1);

				descriptorWrites[0] = write0.get();
				descriptorWrites[1] = write1.get();

				descriptorWrites[0].dstSet=mDescriptorSets[i];
				descriptorWrites[1].dstSet=mDescriptorSets[i];

				mDevice->get().updateDescriptorSets(descriptorWrites, nullptr);
			}

			mGuiDescriptorSet = mVulkan->getDescriptorPool()->allocDescriptorSet(mGuiPipeline->descriptorSetLayouts()[0].get());

			mUi = MixEngine::Instance().getModule<Ui>();
			vk::DescriptorImageInfo info;
			info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			info.sampler = mUi->getFontTexture().getSampler();
			info.imageView = mUi->getFontTexture().getImageView();

			vk::WriteDescriptorSet write{ mGuiDescriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &info };
			mDevice->get().updateDescriptorSets(write, nullptr);
		}

		void StandardRenderer::buildRenderPass() {
			mDepthStencil = Image::CreateDepthStencil(mVulkan->getAllocator(),
													  mSwapchain->extent(),
													  vk::SampleCountFlagBits::e1);
			mDepthStencilView = Image::CreateVkImageView2D(mDevice->get(),
														   mDepthStencil->get(),
														   mDepthStencil->format(),
														   vk::ImageAspectFlagBits::eDepth |
														   vk::ImageAspectFlagBits::eStencil);

			// RenderPass
			mRenderPass = std::make_shared<RenderPass>(mVulkan->getLogicalDevice());

			mRenderPass->addAttachment({
				{0, Attachment::Type::PRESENT, mSwapchain->surfaceFormat().format},
				{1, Attachment::Type::DEPTH_STENCIL, mDepthStencil->format()}
									   });

			Subpass subpass(0);
			subpass.addRef({
				{AttachmentRef::Type::COLOR, 0},
				{AttachmentRef::Type::DEPTH_STENCIL, 1}
						   });

			mRenderPass->addSubpass(subpass);

			mRenderPass->addDependency({
				VK_SUBPASS_EXTERNAL,
				0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::AccessFlags(),
				vk::AccessFlagBits::eColorAttachmentRead |
				vk::AccessFlagBits::eColorAttachmentWrite
									   });
			mRenderPass->create();
		}

		void StandardRenderer::buildFrameBuffer() {
			for (size_t i = 0; i < mSwapchain->imageCount(); ++i) {
				mFrameBuffers.emplace_back(mRenderPass, mVulkan->getSwapchain()->extent());
				mFrameBuffers.back().addAttachments({ mSwapchain->getImageViews()[i], mDepthStencilView });
				mFrameBuffers.back().create();
			}
		}

		void StandardRenderer::buildPipeline() {
			vk::Viewport viewport{
				0.0f, 0.0f,
				static_cast<float>(mSwapchain->width()),
				static_cast<float>(mSwapchain->height()),
				0.0f, 1.0f
			};

			vk::Rect2D scissor{ {0, 0}, mSwapchain->extent() };

			std::ifstream inFile;
			inFile.open("TestResources/pipeline/pipeline.json");
			nlohmann::json json = nlohmann::json::parse(inFile);
			mPipeline = PipelineFactory::CreatePipelineFromJson(mRenderPass, 0, json, viewport, scissor);

			inFile.close();
			// Test Gui
			inFile.open("TestResources/pipeline/gui_pipeline.json");
			json = nlohmann::json::parse(inFile);
			mGuiPipeline = PipelineFactory::CreatePipelineFromJson(mRenderPass, 0, json, viewport, scissor);
		}
	}
}

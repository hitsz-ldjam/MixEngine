#include "MxVkUIRenderer.h"

#include "../Swapchain/MxVkSwapchain.h"
#include "../Pipeline/MxVkPipeline.h"
#include "../../Component/MeshFilter/MxMeshFilter.h"
#include "../../Resource/Shader/MxShaderSource.h"
#include "../../Graphics/Texture/MxTexture.h"
#include "../Pipeline/MxVkPipelineFactory.h"
#include "../../Component/Camera/MxCamera.h"
#include "../../Resource/MxResourceLoader.h"
#include "../Pipeline/MxVkShaderModule.h"
#include "../MxVulkan.h"
#include "../Pipeline/MxVkVertexInput.h"
#include "../MxVkUtils.h"
#include "../Pipeline/MxVkGraphicsPipelineState.h"
//#include "../../RenderAPI/MxVertexDeclaration.h"

namespace Mix {
    namespace Vulkan {
        UIRenderer::UIRenderer(VulkanAPI* _vulkan) :mVulkan(_vulkan) {
            build();
        }

        void UIRenderer::updateBuffers(GUI::UIRenderData& _renderData) {
            size_t vSize = _renderData.drawData->TotalVtxCount * sizeof(ImDrawVert);
            size_t iSize = _renderData.drawData->TotalIdxCount * sizeof(ImDrawIdx);

            if (!mVertexBuffers[mCurrFrame] || mVertexBuffers[mCurrFrame]->size() < vSize)
                mVertexBuffers[mCurrFrame] = std::make_shared<Vulkan::Buffer>(mVulkan->getAllocator(),
                                                                              vk::BufferUsageFlagBits::eVertexBuffer,
                                                                              vk::MemoryPropertyFlagBits::eHostVisible |
                                                                              vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                              vSize);

            if (!mIndexBuffers[mCurrFrame] || mIndexBuffers[mCurrFrame]->size() < iSize)
                mIndexBuffers[mCurrFrame] = std::make_shared<Vulkan::Buffer>(mVulkan->getAllocator(),
                                                                             vk::BufferUsageFlagBits::eIndexBuffer,
                                                                             vk::MemoryPropertyFlagBits::eHostVisible |
                                                                             vk::MemoryPropertyFlagBits::eHostCoherent,
                                                                             iSize);

            size_t vOffset = 0;
            size_t iOffset = 0;

            for (int i = 0; i < _renderData.drawData->CmdListsCount; ++i) {
                auto cmdList = _renderData.drawData->CmdLists[i];
                mVertexBuffers[mCurrFrame]->setData(cmdList->VtxBuffer.Data, vOffset, cmdList->VtxBuffer.size_in_bytes());
                mIndexBuffers[mCurrFrame]->setData(cmdList->IdxBuffer.Data, iOffset, cmdList->IdxBuffer.size_in_bytes());
                vOffset += cmdList->VtxBuffer.size_in_bytes();
                iOffset += cmdList->IdxBuffer.size_in_bytes();
            }
        }

        void UIRenderer::updateTexture(GUI::UIRenderData& _renderData) {
            if (_renderData.fontTextureChanged) {
                WriteDescriptorSet write;
                write = _renderData.fontTexture->getWriteDescriptor(0, vk::DescriptorType::eCombinedImageSampler);

                std::swap(mDescriptorSets[0], mDescriptorSets[1 + mCurrFrame % (mVulkan->getSwapchain()->imageCount() - 1)]);
                mDescriptorSets[0].updateDescriptor(write);
            }
        }

        void Mix::Vulkan::UIRenderer::render(GUI::UIRenderData& _renderData) {
            if (_renderData.drawData->CmdListsCount > 0) {
                mCurrFrame = mVulkan->getSwapchain()->getCurrFrame();

                // Update vertex buffer and indice buffer
                updateBuffers(_renderData);
                updateTexture(_renderData);

                // Draw
                auto& cmd = mVulkan->getCurrDrawCmd();
                cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->get());
                cmd.get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             mPipeline->pipelineLayout(),
                                             0,
                                             mDescriptorSets[0].get(),
                                             nullptr);
                cmd.get().bindVertexBuffers(0, mVertexBuffers[mCurrFrame]->get(), { 0 });
                cmd.get().bindIndexBuffer(mIndexBuffers[mCurrFrame]->get(), 0,
                                          VulkanUtils::GetIndexType(_renderData.indexFormat));

                cmd.get().pushConstants(mPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0,
                                        sizeof(Vector2f), _renderData.scale.linear);
                cmd.get().pushConstants(mPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex,
                                        sizeof(Vector2f), sizeof(Vector2f), _renderData.translate.linear);

                auto drawData = _renderData.drawData;

                int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
                int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

                vk::Viewport viewport;
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = static_cast<float>(fbWidth);
                viewport.height = static_cast<float>(fbHeight);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                cmd.get().setViewport(0, viewport);

                Vector2f clipOff{ drawData->DisplayPos.x, drawData->DisplayPos.y };
                // (0,0) unless using multi-viewports
                Vector2f clipScale{ drawData->FramebufferScale.x, drawData->FramebufferScale.y };
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
                        Vector4f clipRect;
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
                            cmd.get().setScissor(0, 1, &scissor);
                            // Draw
                            cmd.get().drawIndexed(pcmd->ElemCount, 1, pcmd->IdxOffset + idxOffset,
                                                  pcmd->VtxOffset + vtxOffset, 0);
                        }
                    }
                    idxOffset += cmdList->IdxBuffer.Size;
                    vtxOffset += cmdList->VtxBuffer.Size;
                }
            }
        }

        void UIRenderer::build() {
            mDevice = mVulkan->getLogicalDevice();

            // DescriptorSetLayout
            mDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
            mDescriptorSetLayout->setBindings(
                {
                    {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
                }
            );
            mDescriptorSetLayout->create();

            // Pipeline
            GraphicsPipelineStateDesc desc;

            auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/GuiShader.vert");
            auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/GuiShader.frag");
            std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
            std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

            std::array<VertexElement, 3> elements = {
                VertexElement(0, 0, 0, VertexElementType::Float2, VertexElementSemantic::Position, 0),
                VertexElement(0, 1, sizeof(Vector2f), VertexElementType::Float2, VertexElementSemantic::TexCoord, 0),
                VertexElement(0, 2, 2 * sizeof(Vector2f), VertexElementType::UByte4_Norm, VertexElementSemantic::Color, 0)
            };

            desc.vertexDecl = std::make_shared<VertexDeclaration>(elements);

            desc.gpuProgram.vertex = vertShader;
            desc.gpuProgram.fragment = fragShader;

            desc.cullMode = vk::CullModeFlagBits::eNone;
            desc.frontFace = vk::FrontFace::eCounterClockwise;
            desc.polygonMode = vk::PolygonMode::eFill;

            desc.descriptorSetLayouts = { mDescriptorSetLayout };
            desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

            desc.pushConstant.push_back(vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, 2 * sizeof(Vector2f)));

            mPipelineState = std::make_shared<GraphicsPipelineState>(mDevice, desc);

            auto vertexInput = mVulkan->getVertexInputManager().getVertexInput(*desc.vertexDecl, *desc.vertexDecl);
            mPipeline = mPipelineState->getPipeline(mVulkan->getRenderPass(), 0,
                                                    vertexInput, MeshTopology::Triangles_List,
                                                    false, false);

            uint32_t imageCount = mVulkan->getSwapchain()->imageCount();

            // DescriptorSet
            mDescriptorSets = mVulkan->getDescriptorPool()->allocDescriptorSet(*mPipeline->descriptorSetLayouts()[0].get(), imageCount);

            // Buffer
            for (uint32_t i = 0; i < imageCount; ++i) {
                mVertexBuffers.emplace_back(std::make_shared<Buffer>(mVulkan->getAllocator(),
                                            vk::BufferUsageFlagBits::eVertexBuffer,
                                            vk::MemoryPropertyFlagBits::eHostVisible |
                                            vk::MemoryPropertyFlagBits::eHostCoherent,
                                            sDefaultVertexBufferSize));

                mIndexBuffers.emplace_back(std::make_shared<Buffer>(mVulkan->getAllocator(),
                                           vk::BufferUsageFlagBits::eIndexBuffer,
                                           vk::MemoryPropertyFlagBits::eHostVisible |
                                           vk::MemoryPropertyFlagBits::eHostCoherent,
                                           sDefaultVertexBufferSize));
            }

        }
    }
}

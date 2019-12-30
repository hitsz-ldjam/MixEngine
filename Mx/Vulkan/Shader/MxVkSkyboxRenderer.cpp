#include "MxVkSkyboxRenderer.h"
#include "../Pipeline/MxVkGraphicsPipelineState.h"
#include "../../Resource/MxResourceLoader.h"
#include "../../Resource/Shader/MxShaderSource.h"
#include "../../Graphics/Texture/MxTexture.h"
#include "../../Graphics/MxVertexDeclaration.h"
#include "../Descriptor/MxVkDescriptorSet.h"
#include "../MxVulkan.h"
#include "../Swapchain/MxVkSwapchain.h"
#include "../Pipeline/MxVkVertexInput.h"
#include "../Pipeline/MxVkPipeline.h"
#include "../Pipeline/MxVkShaderModule.h"
#include "../../Graphics/Mesh/MxMeshUtils.h"
#include "../../Component/Camera/MxCamera.h"
#include "../MxVkUtils.h"

namespace Mix {
    namespace Vulkan {

        SkyboxShader::~SkyboxShader() {
            mDescriptorSets.clear();
            mDescriptorPool.reset();
        }

        SkyboxShader::SkyboxShader(VulkanAPI* _vulkan) :mVulkan(_vulkan) {
            mDevice = mVulkan->getLogicalDevice();

            // DescriptorSetLayout
            {
                mDescriptorSetLayout = std::make_shared<DescriptorSetLayout>(mDevice);
                mDescriptorSetLayout->setBindings(
                    {
                        {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment}
                    }
                );
                mDescriptorSetLayout->create();
            }

            // Pipeline
            {
                GraphicsPipelineStateDesc desc;

                auto vert = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/SkyBox.vert");
                auto frag = ResourceLoader::Get()->load<ShaderSource>("Resource/Shaders/SkyBox.frag");
                std::shared_ptr<ShaderModule> vertShader = std::make_shared<ShaderModule>(mDevice, *vert);
                std::shared_ptr<ShaderModule> fragShader = std::make_shared<ShaderModule>(mDevice, *frag);

                std::array<VertexElement, 3> elements = {
                    VertexElement(0, 0, 0, VertexElementType::Float3, VertexElementSemantic::Position, 0),
                };

                desc.meshVertexDecl = std::make_shared<VertexDeclaration>(elements);

                desc.gpuProgram.vertex = vertShader;
                desc.gpuProgram.fragment = fragShader;

                desc.cullMode = vk::CullModeFlagBits::eNone;
                desc.frontFace = vk::FrontFace::eCounterClockwise;
                desc.polygonMode = vk::PolygonMode::eFill;

                desc.descriptorSetLayouts = { mDescriptorSetLayout };
                desc.blendStates = { GraphicsPipelineState::DefaultBlendAttachment };

                desc.pushConstant.emplace_back(vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(SkyboxShaderParam)));

                mPipelineState = std::make_shared<GraphicsPipelineState>(mDevice, desc);

                auto vertexInput = mVulkan->getVertexInputManager().getVertexInput(*desc.meshVertexDecl, *desc.meshVertexDecl);

                mPipeline = mPipelineState->getPipeline(mVulkan->getRenderPass(), 0, vertexInput, nullptr, MeshTopology::Triangles_List,
                                                        false, false);
            }

            {
                uint32_t imageCount = mVulkan->getSwapchain()->imageCount();

                // DescriptorSet
                mDescriptorSets = mVulkan->getDescriptorPool()->allocDescriptorSet(*mPipeline->descriptorSetLayouts()[0].get(), imageCount);
            }

            // Model and texture
            {
                mSkyBoxTex = ResourceLoader::Get()->load<CubeMap>("Resource/Textures/Skybox/skybox.ktx");

                auto[vertices, indices] = MeshUtils::Box({ 5.0f, 5.0f, 5.0f });

                mBoxMesh = std::make_shared<Mesh>();
                mBoxMesh->setPositions(vertices);
                mBoxMesh->setIndices(indices, MeshTopology::Points_List, 0, 0);
                mBoxMesh->uploadMeshData(true);
            }

            if (mSkyBoxTex) {
                WriteDescriptorSet write;
                write = mSkyBoxTex->getWriteDescriptor(0, vk::DescriptorType::eCombinedImageSampler);

                mDescriptorSets[0].updateDescriptor(write);
            }

        }

        void SkyboxShader::setCamera(const Camera& _camera) {
            mParam.projection = _camera.getProjMat();
            mParam.projection[1][1] * -1.0f;
            mParam.model = Matrix4::ViewMatrix(Vector3f::Zero, _camera.transform()->forward(), _camera.transform()->up());

            vk::Viewport viewport(
                0.0f, 0.0f,
                _camera.getExtent().x, _camera.getExtent().y,
                0.0f, 1.0f
            );

            vk::Rect2D scissor(
                { 0, 0 },
                vk::Extent2D(_camera.getExtent().x, _camera.getExtent().y)
            );

            auto& cmd = mVulkan->getCurrDrawCmd();
            cmd.get().setViewport(0, viewport);
            cmd.get().setScissor(0, scissor);
        }

        void SkyboxShader::render() {
            // Draw
            auto& cmd = mVulkan->getCurrDrawCmd();
            cmd.get().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->get());
            cmd.get().bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                         mPipeline->pipelineLayout(),
                                         0,
                                         mDescriptorSets[0].get(),
                                         nullptr);
            cmd.get().bindVertexBuffers(0, mBoxMesh->getVertexBuffer()->get(), { 0 });
            cmd.get().bindIndexBuffer(mBoxMesh->getIndexBuffer()->get(), 0,
                                      VulkanUtils::GetIndexType(mBoxMesh->indexFormat()));

            cmd.get().pushConstants(mPipeline->pipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(SkyboxShaderParam), &mParam);

            auto& subMesh = mBoxMesh->getSubMesh()[0];
            cmd.get().drawIndexed(subMesh.indexCount,
                                  1,
                                  subMesh.firstIndex,
                                  subMesh.baseVertex,
                                  0);
        }
    }
}



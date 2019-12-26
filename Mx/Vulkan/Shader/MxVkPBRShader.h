#pragma once
#include "MxVkShaderBase.h"
#include "../Buffers/MxVkUniformBuffer.h"
#include <vulkan/vulkan.hpp>
#include <deque>

namespace Mix {
    class Texture2D;
    class CubeMap;

    namespace Vulkan {
        class Pipeline;
        class RenderPass;
        class Image;
        class Swapchain;
        class DescriptorSetLayout;
        class DescriptorPool;
        class Device;
        class GraphicsPipelineState;
        class Buffer;
        class DynamicUniformBuffer;
        class VertexInput;

        class PBRShader final : public ShaderBase {
        public:
            explicit PBRShader(VulkanAPI* _vulkan);

            ~PBRShader() override;

            void render(ArrayProxy<RenderQueueElement> _elements) override;

            void update(const Shader& _shader) override;

            void beginRender(const Camera& _camera) override;

            void endRender() override;

            uint32_t newMaterial() override;

            void deleteMaterial(uint32_t _id) override;

        private:
            struct MaterialParam {
                Vector4f baseColorFactor;
                Vector4f emissiveFactor;
                Vector4f diffuseFactor;
                Vector4f specularFactor;
                float workflow;
                float hasBaseColorTexture;
                float hasPhysicalDescriptorTexture;
                float hasNormalTexture;
                float hasOcclusionTexture;
                float hasEmissiveTexture;
                float metallicFactor;
                float roughnessFactor;
                float alphaMask;
                float alphaMaskCutoff;
            };

            struct RenderParam {
                Vector4f lightDir;
                Vector4f lightColor;
                float exposure;
                float gamma;
                float prefilteredCubeMipLevels;
                float scaleIBLAmbient;
            };

            void setCamera(const Camera& _camera);

            void beginElement(const RenderElement& _element);

            void endElement();

            bool choosePipeline(const Material& _material, const Mesh& _mesh, uint32_t _submesh);

            void updateTexture(Material& _material);

            void setMaterail(Material& _material);

            void loadGlobalTexture();

            void genCubeMap();

            void buildDescriptorSetLayout();

            void buildPipeline();

            void buildDescriptorSet();

            void buildPropertyBlock();


            std::shared_ptr<Device> mDevice;

            std::shared_ptr<GraphicsPipelineState> mGraphicsPipelineState;

            std::shared_ptr<DescriptorSetLayout> mStaticParamDescriptorSetLayout;
            std::shared_ptr<DescriptorSetLayout> mDynamicPamramDescriptorSetLayout;
            std::shared_ptr<DescriptorPool> mDescriptorPool;
            std::vector<DescriptorSet> mStaticDescriptorSets;
            std::vector<Buffer> mCameraUbo;
            std::vector<Buffer> mRenderParamUbo;
            RenderParam mRenderParam;
            std::vector<DynamicUniformBuffer> mDynamicUniform;

            // PBR tex
            /*struct TexRes {
                std::shared_ptr<Image> image;
                vk::ImageView view;
                vk::Sampler sampler;
            };*/

            std::shared_ptr<CubeMap> mCubeMap;
            std::shared_ptr<CubeMap> mIrradianceMap;
            std::shared_ptr<CubeMap> mPrefilteredMap;
            std::shared_ptr<Texture2D> mBrdfLut;

            void genBrdfLut();

            uint32_t mDefaultMaterialCount = 30;
            std::vector<std::vector<DescriptorSet>> mMaterialDescs;
            std::deque<uint32_t> mUnusedId;

            // Frame rendering info
            std::shared_ptr<VertexInput> mCurrVertexInput;
            std::shared_ptr<Pipeline> mCurrPipeline;
            uint32_t mCurrFrame = 0;
            CommandBufferHandle* mCurrCmd;
        };
    }
}

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

        struct SkyboxShaderParam {
            Matrix4 projection;
            Matrix4 model;
        };

        class SkyboxShader final {
        public:
            ~SkyboxShader();

            explicit SkyboxShader(VulkanAPI* _vulkan);

            void setCamera(const Camera& _camera);

            void render();

        private:
            VulkanAPI* mVulkan = nullptr;
            std::shared_ptr<Device> mDevice;

            std::shared_ptr<GraphicsPipelineState> mPipelineState;
            std::shared_ptr<Pipeline> mPipeline;

            std::shared_ptr<DescriptorPool> mDescriptorPool;
            std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
            std::vector<DescriptorSet> mDescriptorSets;

            std::shared_ptr<CubeMap> mSkyBoxTex;
            std::shared_ptr<Mesh> mBoxMesh;

            SkyboxShaderParam mParam;
        };
    }
}

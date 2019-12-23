#pragma once
#include "MxVkShaderBase.h"
#include "../Buffers/MxVkUniformBuffer.h"
#include <vulkan/vulkan.hpp>
#include <deque>
#include "../../GUI/MxGUi.h"

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

        class UIRenderer {
        public:
            explicit UIRenderer(VulkanAPI* _vulkan);

            void render(GUI::UIRenderData& _renderData);

        private:
            void build();

            static const size_t sDefaultVertexBufferSize = 1024;
            static const size_t sDefaultIndiceBufferSize = 1024;

            VulkanAPI* mVulkan = nullptr;
            std::shared_ptr<Device> mDevice;
            uint32_t mCurrFrame = 0;

            std::shared_ptr<GraphicsPipelineState> mPipelineState;
            std::shared_ptr<Pipeline> mPipeline;

            std::shared_ptr<DescriptorPool> mDescriptorPool;
            std::shared_ptr<DescriptorSetLayout> mDescriptorSetLayout;
            std::vector<DescriptorSet> mDescriptorSets;

            std::vector<std::shared_ptr<Buffer>> mVertexBuffers;
            std::vector<std::shared_ptr<Buffer>> mIndexBuffers;

            void updateBuffers(GUI::UIRenderData& _renderData);
            void updateTexture(GUI::UIRenderData& _renderData);
        };
    }
}

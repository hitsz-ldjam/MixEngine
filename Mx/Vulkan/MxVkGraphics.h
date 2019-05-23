#pragma once
#ifndef MX_VK_GRAPHICS_H_
#define MX_VK_GRAPHICS_H_

#include "../Window/MxWindow.h"
#include "../Object/MxObject.h"

#include "Core/MxVkDef.h"
#include "Core/MxVkCore.h"
#include "Debug/MxVkDebug.h"
#include "Memory/MxVkAllocator.h"
#include "Mesh/MxVkMesh.h"
#include "Pipeline/MxVkShader.h"
#include "Swapchain/MxVkSwapchain.h"
#include "FrameBuffer/MxVkFramebuffer.h"
#include "Pipeline/MxVkRenderPass.h"
#include "Descriptor/MxVkDescriptor.h"
#include "Pipeline/MxVkPipeline.h"
#include "CommandBuffer/MxVkCommand.h"
#include "Buffers/MxVkBuffer.h"
#include "Buffers/MxVkUniformBuffer.h"
#include "Image/MxVkImage.h"

#include <vector>

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

namespace Mix {
    namespace Graphics {
        class Vulkan :public Object {
            MX_DECLARE_RTTI;
            MX_DECLARE_NO_CLASS_FACTORY;
        public:
            Vulkan() {};
            ~Vulkan() {
                destroy();
            }

            void init();

            void setTargetWindow(Window* _window) {
                mWindow = _window;
            }

            void build();

            void update();

            void destroy();

            struct Settings {
                vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
            };

            const vk::Device& getLogicalDevice() const {
                return mCore->getDevice();
            }

            const vk::PhysicalDevice& getPhysicalDevice() const {
                return mCore->getPhysicalDevice();
            }

            std::shared_ptr<Core> getCore() const {
                return mCore;
            }

            std::shared_ptr<DeviceAllocator> getAllocator() const {
                return mAllocator;
            }

            std::shared_ptr<CommandMgr> getCommandMgr() const {
                return mCommandMgr;
            }

        private:
            void buildCore();
            void buildDebugUtils();
            void buildSwapchain();
            void buildDepthStencil();
            void buildRenderPass();
            void buildDescriptorSetLayout();
            void buildShaders();
            void buildPipeline();
            void buildCommandMgr();
            void buildFrameBuffers();

            void buildUniformBuffers();
            void buildCommandBuffers();
            void buildDescriptorSets();

            // test
            // build utils to load resources
            void loadResource();

            void updateCmdBuffer();
            void updateUniformBuffer();

            Window*                 mWindow = nullptr;

            std::shared_ptr<Core>                   mCore;
            std::shared_ptr<DebugUtils>                  mDebug;
            std::shared_ptr<DeviceAllocator>        mAllocator;
            std::shared_ptr<Swapchain>              mSwapchain;

            std::shared_ptr<ShaderMgr>              mShaderMgr;
            std::shared_ptr<PipelineMgr>            mPipelineMgr;

            std::shared_ptr<RenderPass>             mRenderPass;
            std::shared_ptr<DescriptorPool>         mDescriptorPool;
            std::map<std::string, std::shared_ptr<DescriptorSetLayout>>    mDescriptorSetLayout;
            std::shared_ptr<CommandMgr>             mCommandMgr;

            std::shared_ptr<ImageMgr>               mImageMgr;
            // std::shared_ptr<gltf::MeshMgr>          mMeshMgr;

            std::vector<Framebuffer*>       mFramebuffers;
            Image                           mDepthStencil;
            vk::ImageView                   mDepthStencilView;
            std::vector<vk::CommandBuffer>  mCommandBuffers;
            std::vector<vk::DescriptorSet>  mDescriptorSets;

            // todo
            vk::ImageView texImageView;
            vk::Sampler sampler;

            std::vector<std::shared_ptr<Buffer>> cameraUniforms;
            std::vector<std::shared_ptr<DynamicUniformBuffer>> dynamicUniformBuffers;

            Settings mSettings;
        };
    }
}

#endif // !MX_VK_GRAPHICS_H_

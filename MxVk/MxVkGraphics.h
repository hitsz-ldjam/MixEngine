#pragma once
#ifndef MX_VK_GRAPHICS_H_
#define MX_VK_GRAPHICS_H_

#include "../Mx/MxWindow.h"
#include "../Utils/MxUtils.h"
#include "../Mx/MxObject.h"

#include "MxVkDef.h"
#include "MxVkCore.h"
#include "MxVkDebug.h"
#include "MxVkAllocator.h"
#include "MxVkMesh.h"
#include "MxVkShader.h"
#include "MxVkBuffer.h"
#include "MxVkSwapchain.h"
#include "MxVkFramebuffer.h"
#include "MxVkRenderPass.h"
#include "MxVkDescriptor.h"
#include "MxVkPipeline.h"
#include "MxVkCommand.h"

#include <vector>
#include <fstream>

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

            void update(float _deltaTime);

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

        private:
            void updateCmdBuffer(float _deltaTime);
            void updateUniformBuffer(float _deltaTime);

        private:
            Window*                 mWindow = nullptr;

            std::shared_ptr<Core>                   mCore;
            std::shared_ptr<Debug>                  mDebug;
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

            struct Uniform {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
                int index;
            };

            std::vector<std::shared_ptr<Buffer>> uniforms;
            Settings mSettings;
        };
    }
}

#endif // !MX_VK_GRAPHICS_H_

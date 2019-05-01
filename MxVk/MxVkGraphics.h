#pragma once
#ifndef _MX_VK_GRAPHICS_H_
#define _MX_VK_GRAPHICS_H_

#include "../Mx/MxWindow.h"
#include "../Utils/MxUtils.h"
#include "../Mx/MxObject.h"
#include "../Mx/MxRender.h"
#include "../Mx/MxMesh.h"

#include "MxVkDef.h"
#include "MxVkExcept.hpp"
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
#include "MxVkSyncObjMgr.h"
#include "MxVkUniform.h"

#include <vector>
// todo
#include <fstream>

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

namespace Mix {
    namespace Graphics {
        class Graphics {
        public:
            Graphics() {};
            ~Graphics() {
                destroy();
            }

            void init();
            void setTargetWindow(Window* window) {
                mWindow = window;
            }
            void build();
            void update(float deltaTime);

            void destroy();
            struct Settings {
                vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
            };

            /*
            Interfaces
            */
            GameObject* createModelObj(const Utils::GLTFLoader::ModelData& modelData);


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
            void updateCmdBuffer(float deltaTime);
            void updateUniformBuffer(float deltaTime);

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
            std::shared_ptr<gltf::MeshMgr>          mMeshMgr;

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

            std::vector<Buffer*> uniforms;
            Settings mSettings;
        };
    }
}

#endif // !_MX_VK_GRAPHICS_H_

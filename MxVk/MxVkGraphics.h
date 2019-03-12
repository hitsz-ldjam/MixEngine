#pragma once
#ifndef _MX_VK_GRAPHICS_H_
#define _MX_VK_GRAPHICS_H_

#include "../Mx/MxWindow.h"
#include "MxVkDef.h"
#include "MxVkExcept.hpp"
#include "MxVkCore.h"
#include "MxVkDebug.h"
#include "MxVkShader.h"
#include "MxVkBuffer.h"
#include "MxVkSwapchain.h"
#include "MxVkFramebuffer.h"
#include "MxVkRenderPass.h"
#include "MxVkDescriptor.h"
#include "MxVkPipeline.h"
#include "MxVkCommand.h"
#include "MxVkSyncObjMgr.h"

#include <vector>
// todo
#include <fstream>

// todo
const std::vector<Mix::Graphics::Vertex> vertices = {
    {{0.5f,-0.5f,-0.5f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},
    {{0.5f,0.5f,-0.5f},  {1.0f,0.0f,0.0f}, {1.0f,0.0f}},
    {{0.5f,0.5f,0.5f},   {1.0f,0.0f,0.0f}, {1.0f,1.0f}},
    {{0.5f,0.5f,0.5f},   {1.0f,0.0f,0.0f}, {1.0f,1.0f}},
    {{0.5f,-0.5f,0.5f},  {1.0f,0.0f,0.0f}, {0.0f,1.0f}},
    {{0.5f,-0.5f,-0.5f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f}},

    {{0.5f,0.5f,-0.5f},  {0.0f,1.0f,0.0f}, {0.0f,0.0f}},
    {{-0.5f,0.5f,-0.5f}, {0.0f,1.0f,0.0f}, {1.0f,0.0f}},
    {{-0.5f,0.5f,0.5f},  {0.0f,1.0f,0.0f}, {1.0f,1.0f}},
    {{-0.5f,0.5f,0.5f},  {0.0f,1.0f,0.0f}, {1.0f,1.0f}},
    {{0.5f,0.5f,0.5f},   {0.0f,1.0f,0.0f}, {0.0f,1.0f}},
    {{0.5f,0.5f,-0.5f},  {0.0f,1.0f,0.0f}, {0.0f,0.0f}},

    {{-0.5f,0.5f,-0.5f}, {-1.0f,0.0f,0.0f},{0.0f,0.0f}},
    {{-0.5f,-0.5f,-0.5f},{-1.0f,0.0f,0.0f},{1.0f,0.0f}},
    {{-0.5f,-0.5f,0.5f}, {-1.0f,0.0f,0.0f},{1.0f,1.0f}},
    {{-0.5f,-0.5f,0.5f}, {-1.0f,0.0f,0.0f},{1.0f,1.0f}},
    {{-0.5f,0.5f,0.5f},  {-1.0f,0.0f,0.0f},{0.0f,1.0f}},
    {{-0.5f,0.5f,-0.5f}, {-1.0f,0.0f,0.0f},{0.0f,0.0f}},

    {{-0.5f,-0.5f,-0.5f}, {0.0f,-1.0f,0.0f},{0.0f,0.0f}},
    {{0.5f,-0.5f,-0.5f},{0.0f,-1.0f,0.0f},{1.0f,0.0f}},
    {{0.5f,-0.5f,0.5f}, {0.0f,-1.0f,0.0f},{1.0f,1.0f}},
    {{0.5f,-0.5f,0.5f}, {0.0f,-1.0f,0.0f},{1.0f,1.0f}},
    {{-0.5f,-0.5f,0.5f},  {0.0f,-1.0f,0.0f},{0.0f,1.0f}},
    {{-0.5f,-0.5f,-0.5f}, {0.0f,-1.0f,0.0f},{0.0f,0.0f}},

    {{0.5f,-0.5f,0.5f},  {0.0f,0.0f,1.0f}, {0.0f,0.0f}},
    {{0.5f,0.5f,0.5f},   {0.0f,0.0f,1.0f}, {1.0f,0.0f}},
    {{-0.5f,0.5f,0.5f},  {0.0f,0.0f,1.0f}, {1.0f,1.0f}},
    {{-0.5f,0.5f,0.5f},  {0.0f,0.0f,1.0f}, {1.0f,1.0f}},
    {{-0.5f,-0.5f,0.5f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f}},
    {{0.5f,-0.5f,0.5f},  {0.0f,0.0f,1.0f}, {0.0f,0.0f}},

    {{0.5f,0.5f,-0.5f},  {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
    {{0.5f,-0.5f,-0.5f}, {0.0f,0.0f,-1.0f},{1.0f,0.0f}},
    {{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,1.0f}},
    {{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,-1.0f},{1.0f,1.0f}},
    {{-0.5f,0.5f,-0.5f}, {0.0f,0.0f,-1.0f},{0.0f,1.0f}},
    {{0.5f,0.5f,-0.5f},  {0.0f,0.0f,-1.0f},{0.0f,0.0f}},
};

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

namespace Mix {
    namespace Graphics {
        class Graphics :GraphicsBase {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
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

            void buildVertexBuffers();
            void buildUniformBuffers();
            void buildCommandBuffers();
            void buildDescriptorSets();

        private:
            Window*                 mWindow = nullptr;

            Core*                   mCore = nullptr;
            Debug*                  mDebug = nullptr;
            Swapchain*              mSwapchain = nullptr;

            ShaderMgr*              mShaderMgr = nullptr;
            PipelineMgr*            mPipelineMgr = nullptr;
            SyncObjectMgr*          mSyncObjMgr = nullptr;

            RenderPass*             mRenderPass = nullptr;
            DescriptorPool*         mDescriptorPool = nullptr;
            DescriptorSetLayout*    mDescriptorSetLayout = nullptr;
            CommandMgr*             mCommandMgr = nullptr;

            std::vector<Framebuffer*>       mFramebuffers;
            Image                           mDepthStencil;
            vk::ImageView                   mDepthStencilView;
            std::vector<vk::CommandBuffer>  mCommandBuffers;
            std::vector<vk::DescriptorSet>  mDescriptorSets;

            // todo
            Buffer* box;
            struct Uniform {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
            };
            std::vector<Buffer*> uniforms;

            Settings mSettings;
        };
    }
}

#endif // !_MX_VK_GRAPHICS_H_

#pragma once
#ifndef MX_GRAPHICS_H_
#define MX_GRAPHICS_H_

#include "../Engine/MxModuleBase.h"
#include "MxShader.h"
#include "../Vulkan/MxVulkan.h"

namespace Mix {
    struct FrameRenderInfo;
    class Window;
    struct FrameSceneInfo;

    namespace Vulkan {
        class VulkanAPI;
        class UIRenderer;
        class SkyboxShader;
    }

    class Graphics :public ModuleBase {
    public:
        static Graphics* Get();

        ~Graphics();

        void load() override;

        void init() override;

        Vulkan::VulkanAPI& getRenderApi() const { return *mVulkan; }

        void update();

        void render(FrameRenderInfo& _frameInfo);

        std::shared_ptr<Shader> findShader(const std::string& _name);

    private:
        void initRenderAPI(Window* _window);

        void loadShader();

        void addShader(const std::string _name, const std::shared_ptr<Vulkan::ShaderBase>& _shader);

        std::unique_ptr<Vulkan::VulkanAPI> mVulkan;

        std::unordered_map<uint32_t, std::shared_ptr<Shader>> mShaders;
        std::unordered_map<std::string, uint32_t> mShaderNameMap;

        std::shared_ptr<Vulkan::UIRenderer> mUiRenderer;
        std::shared_ptr<Vulkan::SkyboxShader> mSkyboxRenderer;
    };
}

#endif

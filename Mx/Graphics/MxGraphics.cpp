#include "MxGraphics.h"
#include "../../MixEngine.h"
#include "../Vulkan/MxVulkan.h"
#include "../Vulkan/Shader/MxVkStandardShader.h"
#include "../GameObject/MxGameObject.h"
#include <queue>
#include "../Scene/MxSceneManager.hpp"


namespace Mix {

    Graphics* Graphics::Get() {
        return MixEngine::Instance().getModule<Graphics>();
    }

    Graphics::~Graphics() {
        mVulkan->waitDeviceIdle();
        mShaderNameMap.clear();
        mShaders.clear();
        mVulkan.reset();
    }

    void Graphics::load() {
        initRenderAPI(Window::Get());
    }

    void Graphics::init() {
        loadShader();
    }

    void Graphics::update() {
    }

    void Graphics::render() {
        auto renderInfo = SceneManager::Get()->getActiveScene()->_getRendererInfoPerFrame();
        mVulkan->beginRender();
        {
            mShaders[0]->_render(renderInfo);
        }
        mVulkan->endRender();
    }

    Shader* Graphics::findShader(const std::string& _name) {
        if (mShaderNameMap.count(_name))
            return mShaders[mShaderNameMap[_name]].get();
        return nullptr;
    }

    void Graphics::initRenderAPI(Window* _window) {
        std::vector<const char*> instanceExtsReq = _window->getRequiredInstanceExts();
        instanceExtsReq.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        std::vector<const char*> deviceExtsReq;
        deviceExtsReq.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        std::vector<const char*> layersReq;
        layersReq.push_back("VK_LAYER_LUNARG_standard_validation");

        Vulkan::VulkanSettings settings;
        settings.appInfo.appName = "Demo";
        settings.appInfo.appVersion = Version(0, 0, 1);
        settings.debugMode = true;
        settings.instanceExts = instanceExtsReq;
        settings.deviceExts = deviceExtsReq;
        settings.validationLayers = layersReq;
        settings.physicalDeviceIndex = 0;

        auto vulkan = std::make_unique<Vulkan::VulkanAPI>();
        vulkan->init();
        vulkan->setTargetWindow(_window);
        vulkan->build(settings);

        mVulkan = std::move(vulkan);
    }

    void Graphics::loadShader() {
        auto shader = std::make_shared<Vulkan::StandardShader>(mVulkan.get());
        mShaderNameMap["Standard"] = 0;
        mShaders[0].reset(new Shader(shader, 0, "Standard", shader->getShaderPropertySet(), shader->getMaterialPropertySet()));
    }
}

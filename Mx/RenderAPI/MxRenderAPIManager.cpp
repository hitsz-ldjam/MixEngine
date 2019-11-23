#include "MxRenderAPIManager.h"
#include "../Vulkan/MxVulkan.h"
#include "../../MixEngine.h"

namespace Mix {
    RenderAPIManager* RenderAPIManager::Get() {
        return MixEngine::Instance().getModule<RenderAPIManager>();
    }

    RenderAPIManager::~RenderAPIManager() {
        if (mActiveRenderAPI) {
            mActiveRenderAPI->destroy();
            delete mActiveRenderAPI;
        }
    }

    void RenderAPIManager::loadRenderAPI(const std::string& _name, Window* _window) {
        // We only support Vulkan API so far, so we ignore the parameter _name
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

        auto vulkan = new Vulkan::VulkanAPI();
        vulkan->init();
        vulkan->setTargetWindow(_window);
        vulkan->build(settings);

        mActiveRenderAPI = vulkan;
    }
}

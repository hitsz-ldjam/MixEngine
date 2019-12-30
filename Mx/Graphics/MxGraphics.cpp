#include "MxGraphics.h"
#include "../../MixEngine.h"
#include "../Vulkan/MxVulkan.h"
#include "../Vulkan/Shader/MxVkStandardShader.h"
#include "../GameObject/MxGameObject.h"
#include <queue>
#include "../Scene/MxSceneManager.h"
#include "MxRenderQueue.h"
#include "../Component/Renderer/MxRenderer.h"
#include "../Component/MeshFilter/MxMeshFilter.h"
#include "../Component/Camera/MxCamera.h"
#include "../Vulkan/Shader/MxVkPBRShader.h"
#include "../Vulkan/Shader/MxVkUIRenderer.h"
#include "../Time/MxTime.h"
#include "../Vulkan/Shader/MxVkBallShader.h"
#include "../Vulkan/Shader/MxVkSkyboxRenderer.h"


namespace Mix {

    Graphics* Graphics::Get() {
        return MixEngine::Instance().getModule<Graphics>();
    }

    Graphics::~Graphics() {
        mVulkan->waitDeviceIdle();
        mShaderNameMap.clear();
        mShaders.clear();
        mUiRenderer.reset();
        mSkyboxRenderer.reset();
        mVulkan.reset();
    }

    void Graphics::load() {
        initRenderAPI(Window::Get());
    }

    void Graphics::init() {
        loadShader();
    }

#define TIME_BEGIN auto sxxxstart = Time::RealTime()

#define TIME_VALUE (Time::RealTime() - sxxxstart)

#define TIME_LOG(m) std::cout<<(m)<<" : "<<TIME_VALUE<<std::endl

#define TIME_TICK sxxxstart = Time::RealTime()

#define TIME_SEPR std::cout<<"------------------"<<std::endl

    void Graphics::update() {
    }

    void Graphics::render(FrameRenderInfo& _frameInfo) {
        for (auto& shader : mShaders)
            shader.second->update();

        auto& renderInfo = *_frameInfo.sceneInfo;

        Camera& camera = *renderInfo.camera;
        auto frustum = camera.getFrustum();
        Vector3f cameraPos = renderInfo.camera->transform()->getPosition();

        std::vector<RenderElement> renderElements;

        RenderQueue transparentQueue(RenderQueue::SortType_BackToFront);
        RenderQueue opaqueQueue(RenderQueue::SortType_FrontToBack);

        for (auto& renderer : renderInfo.renderers) {
            if (!frustum.contains(renderer->transform()->getPosition()))
                continue;

            auto mesh = renderer->getGameObject()->getComponent<MeshFilter>()->getMesh();
            if (mesh) {
                auto& materials = renderer->getMaterials();

                uint32_t count = std::min(mesh->subMeshCount(), static_cast<uint32_t>(materials.size()));
                for (uint32_t i = 0; i < count; ++i) {
                    RenderElement re;
                    re.transform = renderer->transform();
                    re.material = materials[i];
                    re.mesh = mesh;
                    re.submesh = i;

                    renderElements.push_back(re);
                }
            }
        }

        for (auto& element : renderElements) {
            float dist = (element.transform->getPosition() - cameraPos).length();

            switch (element.material->getRenderType()) {
            case RenderType::Transparent: transparentQueue.push(&element, dist); break;
            default:
            case RenderType::Opaque: opaqueQueue.push(&element, dist); break;
            }
        }

        transparentQueue.sort();
        opaqueQueue.sort();

        auto& transparentElements = transparentQueue.getSortedElements();
        auto& opaqueElements = opaqueQueue.getSortedElements();

        mVulkan->beginRender();

        mSkyboxRenderer->setCamera(camera);
        mSkyboxRenderer->render();

        // Render all opaque elements
        if (!opaqueElements.empty()) {
            uint32_t id;
            auto l = opaqueElements.begin();
            auto r = l + 1;

            while (l != opaqueElements.end()) {
                id = l->shaderId;
                while (r != opaqueElements.end() && id == r->shaderId)
                    ++r;

                mShaders[id]->beginRender(camera);
                mShaders[l->shaderId]->render({ l, r });
                mShaders[id]->endRender();

                l = r;
            }
        }

        // Render all transparent elements
        if (!transparentElements.empty()) {
            uint32_t id;
            auto l = transparentElements.begin();
            auto r = l + 1;

            while (l != transparentElements.end()) {
                id = l->shaderId;
                while (r != transparentElements.end() && id == r->shaderId)
                    ++r;

                mShaders[id]->beginRender(camera);
                mShaders[l->shaderId]->render({ l, r });
                mShaders[id]->endRender();

                l = r;
            }
        }

        // UI
        GUI::UIRenderData renderData;
        bool renderUi = GUI::Get()->getRenderData(renderData);
        if (renderUi)
            mUiRenderer->render(renderData);
        mVulkan->endRender();
    }

    std::shared_ptr<Shader> Graphics::findShader(const std::string& _name) {
        if (mShaderNameMap.count(_name))
            return mShaders[mShaderNameMap[_name]];
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
        auto standard = std::make_shared<Vulkan::StandardShader>(mVulkan.get());
        addShader("Standard", standard);

        auto pbr = std::make_shared<Vulkan::PBRShader>(mVulkan.get());
        addShader("PBR", pbr);

        auto ball = std::make_shared<Vulkan::BallShader>(mVulkan.get());
        addShader("BallShader", ball);

        mSkyboxRenderer = std::make_shared<Vulkan::SkyboxShader>(mVulkan.get());

        mUiRenderer = std::make_shared<Vulkan::UIRenderer>(mVulkan.get());
    }

    void Graphics::addShader(const std::string _name, const std::shared_ptr<Vulkan::ShaderBase>& _shader) {
        if (mShaderNameMap.count(_name))
            return;

        mShaderNameMap[_name] = mShaders.size() + 1;
        mShaders[mShaderNameMap[_name]] = std::shared_ptr<Shader>(new Shader(_shader, mShaderNameMap[_name], _name, _shader->getShaderPropertySet(), _shader->getMaterialPropertySet()));
    }
}

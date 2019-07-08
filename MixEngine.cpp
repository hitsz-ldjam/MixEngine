#include "MixEngine.h"

#include "Mx/Time/MxTime.h"
#include "Mx/Audio/MxAudio.hpp"
#include "Mx/Input/MxInput.h"
#include "Mx/Window/MxWindow.h"
#include "Mx/Vulkan/MxVkGraphics.h"
#include "Mx/Vulkan/Renderer/PresetRenderer/MxVkStandardRenderer.h"
#include "Mx/Resource/MxResourceLoader.h"
#include "Mx/GUI/MxUi.h"

namespace Mix {
    MixEngine::MixEngine(int _argc, char** _argv) : mQuit(false),
                                                    mWindow(nullptr),
                                                    mDebugScene("Debug Scene") {}

    MixEngine::~MixEngine() {
        mModuleHolder.get<Audio::Core>()->release();
        delete mWindow;
        SDL_Quit();
    }

    int MixEngine::exec() {
        try {
            awake();
            init();
            SDL_Event event;
            while(!mQuit) {
                while(SDL_PollEvent(&event)) {
                    process(event);
                }
                update();
                lateUpdate();
                render();
            }
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void MixEngine::awake() {
        if(SDL_Init(SDL_INIT_VIDEO))
            throw ThirdPartyLibInitError("SDL2");

        auto fmodCore = mModuleHolder.add<Audio::Core>();
        fmodCore->awake();

        Time::Awake();
        Input::Init();

        // todo test graphics
        mWindow = new Window("Mix Engine Demo", {800, 600});
        auto instanceExtsReq = mWindow->getRequiredInstanceExts();
        instanceExtsReq.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        std::vector<const char*> deviceExtsReq;
        deviceExtsReq.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        std::vector<const char*> layersReq;
        layersReq.push_back("VK_LAYER_LUNARG_standard_validation");

        Graphics::VulkanSettings settings;
        settings.appInfo.appName = "Demo";
        settings.appInfo.appVersion = Version::MakeVersion(0, 0, 1);
        settings.debugMode = true;
        settings.instanceExts = instanceExtsReq;
        settings.deviceExts = deviceExtsReq;
        settings.validationLayers = layersReq;
        settings.physicalDeviceIndex = 0;

        auto vulkan = mModuleHolder.add<Graphics::Vulkan>();
        vulkan->init();

        Debug::Log::Info("Instance Extensions:");

        auto instanceExts = Graphics::Vulkan::GetAllSupportedInstanceExts();
        for(auto& ext : instanceExts) {
            Debug::Log::Info("--%s", ext.extensionName);
        }

        Debug::Log::Info("Validation Layers:");

        auto layers = Graphics::Vulkan::GetAllSupportedLayers();
        for(auto& layer : layers) {
            Debug::Log::Info("--%s", layer.layerName);
        }

        vulkan->setTargetWindow(mWindow);
        vulkan->build(settings);

        auto ui = mModuleHolder.add<Ui>();
        ui->init();

        auto resourceLoader = mModuleHolder.add<Resource::ResourceLoader>();
        resourceLoader->init();

        auto mainRendererIndex = vulkan->addRenderer<Graphics::StandardRenderer>();
        vulkan->setActiveRenderer(mainRendererIndex);

        mDebugScene.awake();
    }

    void MixEngine::init() {
        mDebugScene.init();
    }

    void MixEngine::process(const SDL_Event& _event) {
        switch(_event.type) {
            case SDL_KEYDOWN:
            {
                SDL_Scancode code = _event.key.keysym.scancode;
                Input::anyKey = true;
                Input::keyEvent[code] |= Input::PRESSED_MASK;
                if(!_event.key.repeat) {
                    Input::anyKeyDown = true;
                    Input::keyEvent[code] |= Input::FIRST_PRESSED_MASK;
                }
                break;
            }
            case SDL_KEYUP:
            {
                Input::keyEvent[_event.key.keysym.scancode] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                // if(event.button.clicks == 1)
                Input::mouseButtonEvent[_event.button.button - 1] |= Input::FIRST_PRESSED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                Input::mouseButtonEvent[_event.button.button - 1] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                break; // Use SDL_GetMouseState() for real-time mouse state info instead
            }
            case SDL_MOUSEWHEEL:
            {
                int deltaY = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
                deltaY *= _event.wheel.y;
                Input::mouseScrollDelta += Math::Vector2i(_event.wheel.x, deltaY);
                break;
            }
            case SDL_QUIT:
            {
                mQuit = true;
                //for(auto be : behaviours)
                //    if(!be->onApplicationQuit())
                //        mQuit = false;
                break;
            }
            default:
                break;
        }
    }

    void MixEngine::update() {
        Time::Update();

        // calculate fps
        static auto startTp = Time::TotalTime();
        if(++mFrameCount > mFrameSampleRate) {
            mFramePerSecond = mFrameCount / (Time::TotalTime() - startTp);
            startTp = Time::TotalTime();
            mFrameCount = 0u;
        }
        mWindow->setTitle(std::to_string(mFramePerSecond));
        // -----

        mDebugScene.update();

        for(auto i = 0u; i < Time::sFixedClampedSteps; ++i)
            fixedUpdate();
    }

    void MixEngine::fixedUpdate() {
        // todo: call bullet3

        mDebugScene.fixedUpate();
    }

    void MixEngine::lateUpdate() {
        mDebugScene.lateUpate();

        mModuleHolder.get<Audio::Core>()->update();
        Input::Reset();
    }

    void MixEngine::render() {
        mModuleHolder.get<Ui>()->update();
        mModuleHolder.get<Ui>()->render();
        mModuleHolder.get<Graphics::Vulkan>()->update();
        mModuleHolder.get<Graphics::Vulkan>()->render();
    }
}

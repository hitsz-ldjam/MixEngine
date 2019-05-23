#include "MixEngine.h"

#include "Mx/Exceptions/MxExceptions.hpp"
#include "Mx/Input/MxInput.h"
#include "Mx/Time/MxTime.h"

#include <iostream>

namespace Mix {
    MixEngine::MixEngine(int _argc, char** _argv) {
        mQuit = false;
        mFmodCore = nullptr;
    }

    MixEngine::~MixEngine() {
        if (mFmodCore)
            mFmodCore->release();
        mFmodCore = nullptr;

        delete mCamera;
        delete mGameObject;
        delete mResources;
        delete mVulkan;
        delete mWindow;
        SDL_Quit();
    }

    int MixEngine::exec() {
        try {
            init();
            SDL_Event event;
            while (!mQuit) {
                while (SDL_PollEvent(&event))
                    process(event);
                update();
                lateUpdate();
                render();
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void MixEngine::init() {
        mQuit = false;

        // initialize sdl
        if (SDL_Init(SDL_INIT_VIDEO))
            throw SdlInitializationError();

        // initialize fmod
        auto result = FMOD::System_Create(&mFmodCore);
        if (result != FMOD_OK)
            throw FmodInitializationError();
        result = mFmodCore->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
            throw FmodInitializationError();

        // initialize engine
        Input::Init();
        Time::Init();

        // initialize behaviours
        // todo: delete debug code
        // mScene.init();

        // todo test graphics
        mWindow = new Window("Surprise! Mother Fucker!", { 680,400 }, SDL_WINDOW_VULKAN);

        mVulkan = new Graphics::Vulkan();
        mVulkan->init();
        mVulkan->setTargetWindow(mWindow);
        mVulkan->build();

        mResources = new Resource::Resources();
        mResources->init();

        // add a model
        mCamera = new GameObject("Camera");
        mGameObject = dynamic_cast<GameObject*>(mResources->load("E:/Git/vulkan-learning-master/res/models/gltfSample/GearboxAssy/glTF/GearboxAssy.gltf"));
    }

    void MixEngine::process(const SDL_Event& _event) {
        switch (_event.type) {
        case SDL_KEYDOWN:
        {
            SDL_Scancode code = _event.key.keysym.scancode;
            Input::anyKey = true;
            Input::keyEvent[code] |= Input::PRESSED_MASK;
            if (!_event.key.repeat) {
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
            break; // Use SDL_GetMouseState() for real-time info instead
        }
        case SDL_MOUSEWHEEL:
        {
            int deltaY = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
            deltaY *= _event.wheel.y;
            Input::mouseScrollDelta += glm::ivec2(_event.wheel.x, deltaY);
            break;
        }
        case SDL_QUIT:
        {
            mQuit = true;
            /*for(auto be : behaviours)
                if(!be->onApplicationQuit())
                    mQuit = false;*/
            break;
        }
        default:
            break;
        }
    }

    void MixEngine::update() {
        Time::Tick();

        // update behaviours
        // todo: delete debug code
        // mScene.update();

        // todo: smooth and reset smoothing
        for (int i = 0; i < Time::sFixedClampedSteps; ++i) {
            fixedUpdate();
        }
    }

    void MixEngine::fixedUpdate() {
        // todo: call bullet3

        // fixed update behaviours
        // todo: delete debug code
        // mScene.fixedUpate();
    }

    void MixEngine::lateUpdate() {
        // late update beahviours
        // todo: delete debug code
        // mScene.lateUpate();

        Input::Reset();

        mFmodCore->update();
    }

    // todo: call vulkan here
    void MixEngine::render() {
        auto tran = mCamera->getComponent<Mix::Transform>();
        tran->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_W))
            tran->translate(Mix::Axis::WorldForward * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_S))
            tran->translate(-Mix::Axis::WorldForward * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_D))
            tran->translate(Mix::Axis::WorldRight * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_A))
            tran->translate(-Mix::Axis::WorldRight * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_SPACE))
            tran->translate(Mix::Axis::WorldUp * (Time::DeltaTime() * 5.0f), Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_LCTRL))
            tran->translate(-Mix::Axis::WorldUp * (Time::DeltaTime() * 5.0f), Mix::Space::WORLD);
        mVulkan->update();
    }
}

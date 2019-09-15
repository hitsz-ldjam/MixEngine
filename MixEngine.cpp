#include "MixEngine.h"
#include "Mx/Window/MxWindow.h"
#include "Mx/Time/MxTime.h"
#include "Mx/Input/MxInput.h"
#include "Mx/Audio/MxAudio.hpp"
#include "Mx/Physics/MxPhysicsWorld.h"
#include "Mx/Vulkan/Shader/MxVkStandardShader.h"
#include "Mx/GUI/MxUi.h"
#include "Mx/Resource/MxResourceLoader.h"
#include "Mx/Graphics/MxGraphics.h"
#include "Mx/Scene/MxSceneManager.hpp"
#include "Mx/Engine/MxPlatform.h"
#include "MxApplicationBase.h"

namespace Mix {
    MixEngine::MixEngine(int _argc, char** _argv) {
        for (int i = 0; i < _argc; ++i)
            mCommandLines.emplace_back(_argv[i]);
    }

    void MixEngine::requestQuit() {
        Platform::PushQuitEvent();
    }

    void MixEngine::setFPSLimit(uint32_t _limit) {
        mFPSLimit = _limit;
        if (mFPSLimit > 0)
            mFrameStep = 1 / mFPSLimit;
    }

    MixEngine::~MixEngine() {
        //mModuleHolder.get<Audio::Core>()->release();
        mModuleHolder.clear();
        Platform::ShutDown();
    }

    int MixEngine::execute(std::shared_ptr<ApplicationBase> _app) {
        if (_app == nullptr) {
            throw std::runtime_error("No application specified.");
        }
        mApp = std::move(_app);

        Time::Awake();
        Platform::Initialize();
        Platform::QuitEvent.connect(std::bind(&MixEngine::onQuitRequested, this));

        mApp->startUp(getCommandLines());

        try {
            loadModule();
            initModule();
            loadMainScene();

            while (!mQuit) {
                awake();
                init();

                while (mRunning) {
                    Platform::Update();
                    Time::Tick();

                    // Limit FPS if limit was set.
                    if (mFPSLimit > 0) {
                        float currentTime = Time::TotalTime();
                        float nextFrameTime = mLastFrameTime + mFrameStep;
                        while (nextFrameTime > currentTime) {
                            uint32_t waitTime = (nextFrameTime - currentTime) * 1000;

                            // If waiting for longer, sleep
                            if (waitTime >= 2000) {
                                Platform::Sleep(waitTime);
                                currentTime = Time::TotalTime();
                            }
                        }

                        mLastFrameTime = currentTime;
                    }

                    // Calculate fps
                    static auto startTp = Time::RealTime();
                    if (++mFrameCount > mFrameSampleRate) {
                        mFramePerSecond = mFrameCount / (Time::RealTime() - startTp);
                        startTp = Time::RealTime();
                        mFrameCount = 0u;
                    }


                    Window::Get()->setTitle(std::to_string(mFramePerSecond));
                    update();
                    lateUpdate();
                    render();
                    postRender();
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void MixEngine::loadMainScene() {
        auto sceneManager = mModuleHolder.get<SceneManager>();

        auto scene = sceneManager->createScene("MainScene");
        auto filler = std::make_shared<DefaultSceneFiller>();
        scene->setFiller(filler);
        sceneManager->loadScene(scene->getIndex());
        sceneManager->setActiveScene(scene);

        mApp->onMainSceneCreated();
    }

    void MixEngine::awake() {
        Time::Tick();
        mApp->onAwake();
        mModuleHolder.get<SceneManager>()->sceneAwake();
    }

    void MixEngine::init() {
        mApp->onInit();
        mModuleHolder.get<SceneManager>()->sceneInit();
    }

    void MixEngine::update() {
        mApp->onUpdate();
        mModuleHolder.get<Physics::World>()->sync(Time::FixedDeltaTime(), Time::SmoothingFactor());
        mModuleHolder.get<SceneManager>()->sceneUpdate();

        for (auto i = 0u; i < Time::sFixedClampedSteps; ++i) {
            fixedUpdate();
        }
    }

    void MixEngine::fixedUpdate() {
        mApp->onFixedUpdate();
        mModuleHolder.get<Physics::World>()->step(Time::FixedDeltaTime());
        mModuleHolder.get<SceneManager>()->sceneFixedUpdate();
    }

    void MixEngine::lateUpdate() {
        mApp->onLateUpdate();
        mModuleHolder.get<SceneManager>()->sceneLateUpdate();
        mModuleHolder.get<SceneObjectManager>()->lateUpdate();
        mModuleHolder.get<Audio::Core>()->update();
    }

    void MixEngine::loadModule() {
        mModuleHolder.add<Window>("Mix Engine Demo", Vector2i{ 1024, 760 }, WindowFlag::VULKAN | WindowFlag::SHOWN);
        mModuleHolder.add<Input>()->load();
        mModuleHolder.add<Audio::Core>()->load();
        mModuleHolder.add<Physics::World>()->load();
        mModuleHolder.add<Graphics>()->load();
        mModuleHolder.add<ResourceLoader>()->load();
        mModuleHolder.add<SceneObjectManager>()->load();
        mModuleHolder.add<SceneManager>()->load();

        mApp->onModuleLoaded();
    }

    void MixEngine::initModule() {
        auto modules = mModuleHolder.getAllOrdered();
        for (auto m : modules)
            m->init();

        mApp->onMoudleInitialized();
    }

    void MixEngine::onQuitRequested() {
        if (mApp->onQuitRequested())
            quit();
    }

    void MixEngine::render() {
        mApp->onRender();
        mModuleHolder.get<Graphics>()->update();
        mModuleHolder.get<Graphics>()->render();
    }

    void MixEngine::postRender() {
        mApp->onPostRender();
        mModuleHolder.get<Input>()->nextFrame();
    }

}

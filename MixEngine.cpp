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
#include "Mx/Engine/MxPlantform.h"

namespace Mix {
    MixEngine::MixEngine(int _argc, char** _argv) : mQuit(false) {
        Plantform::Initialize();
        Plantform::RequireQuitEvent.connect(std::bind(&MixEngine::shutDown, this));
    }

    MixEngine::~MixEngine() {
        //mModuleHolder.get<Audio::Core>()->release();
        mModuleHolder.clear();
        Plantform::ShutDown();
    }

    int MixEngine::exec() {
        try {
            awake();
            init();
            while(!mQuit) {
                Plantform::Update();
                update();
                lateUpdate();
                render();
                postRender();
            }
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void MixEngine::awake() {
        Time::Awake();
        mModuleHolder.add<Window>("Mix Engine Demo", Math::Vector2i{1024, 760}, WindowFlag::VULKAN | WindowFlag::SHOWN);
        mModuleHolder.add<Input>()->awake();
        mModuleHolder.add<Audio::Core>()->awake();
        mModuleHolder.add<Physics::World>()->awake();
        mModuleHolder.add<Graphics>()->awake();
        mModuleHolder.add<ResourceLoader>()->awake();
        mModuleHolder.add<SceneManager>()->awake(); // might be buggy
    }

    void MixEngine::init() {
        auto modules = mModuleHolder.getAllOrdered();
        for(auto m : modules)
            m->init();
    }

    void MixEngine::update() {
        Time::Tick();
        mModuleHolder.get<Physics::World>()->sync(Time::FixedDeltaTime(), Time::SmoothingFactor());
        //mModuleHolder.get<Ui>()->update();

        // calculate fps
        static auto startTp = Time::TotalTime();
        if(++mFrameCount > mFrameSampleRate) {
            mFramePerSecond = mFrameCount / (Time::TotalTime() - startTp);
            startTp = Time::TotalTime();
            mFrameCount = 0u;
        }
        Window::Get()->setTitle(std::to_string(mFramePerSecond));
        // -----

        mModuleHolder.get<SceneManager>()->update();

        for(auto i = 0u; i < Time::sFixedClampedSteps; ++i)
            fixedUpdate();
    }

    void MixEngine::fixedUpdate() {
        mModuleHolder.get<Physics::World>()->step(Time::FixedDeltaTime());

        mModuleHolder.get<SceneManager>()->fixedUpate();
    }

    void MixEngine::lateUpdate() {
        mModuleHolder.get<SceneManager>()->lateUpate();

        mModuleHolder.get<Audio::Core>()->update();
    }

    void MixEngine::render() {
        mModuleHolder.get<Graphics>()->update();
        mModuleHolder.get<Graphics>()->render();
    }

    void MixEngine::postRender() {
        mModuleHolder.get<Input>()->nextFrame();
    }

}

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

namespace Mix {
	MixEngine::MixEngine(int _argc, char** _argv) : mQuit(false),
		mDebugScene("Debug Scene") {
	}

	MixEngine::~MixEngine() {
		//mModuleHolder.get<Audio::Core>()->release();
		SDL_Quit();
	}

	int MixEngine::exec() {
		try {
			awake();
			init();
			SDL_Event event;
			while (!mQuit) {
				while (SDL_PollEvent(&event)) {
					process(event);
				}
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

	void MixEngine::awake() {
		mModuleHolder.add<Window>("Mix Engine Demo", Math::Vector2i{ 1024,760 }, WindowFlag::VULKAN | WindowFlag::SHOWN);
		mModuleHolder.add<Audio::Core>()->awake();
		mModuleHolder.add<Physics::World>()->awake();
		mModuleHolder.add<Graphics>()->awake();
		mModuleHolder.add<ResourceLoader>()->awake();
		Time::Awake();
		Input::Awake();

		Window::SetRelativeMouseMode(true);
	}

	void MixEngine::init() {
		auto modules = mModuleHolder.getAllOrdered();
		for (auto m : modules)
			m->init();

		mDebugScene.awake();
		mDebugScene.init();
	}

	void MixEngine::process(const SDL_Event& _event) {
		if (_event.type == SDL_QUIT) {
			mQuit = true;
			return;
		}

		// mModuleHolder.get<Ui>()->process(_event);
		Input::Process(_event);
	}

	void MixEngine::update() {
		Time::Tick();
		mModuleHolder.get<Physics::World>()->sync(Time::FixedDeltaTime(), Time::SmoothingFactor());
		//mModuleHolder.get<Ui>()->update();

		// calculate fps
		static auto startTp = Time::TotalTime();
		if (++mFrameCount > mFrameSampleRate) {
			mFramePerSecond = mFrameCount / (Time::TotalTime() - startTp);
			startTp = Time::TotalTime();
			mFrameCount = 0u;
		}
		Window::Get()->setTitle(std::to_string(mFramePerSecond));
		// -----

		mDebugScene.update();

		for (auto i = 0u; i < Time::sFixedClampedSteps; ++i)
			fixedUpdate();
	}

	void MixEngine::fixedUpdate() {
		mModuleHolder.get<Physics::World>()->step(Time::FixedDeltaTime());

		mDebugScene.fixedUpate();
	}

	void MixEngine::lateUpdate() {
		mDebugScene.lateUpate();

		mModuleHolder.get<Audio::Core>()->update();
		Input::Reset();
	}

	void MixEngine::render() {
		mModuleHolder.get<Graphics>()->update();
		mModuleHolder.get<Graphics>()->render();
	}
}

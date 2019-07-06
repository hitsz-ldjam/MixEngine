#include "MixEngine.h"

#include "Mx/Exceptions/MxExceptions.hpp"
#include "Mx/Input/MxInput.h"
#include "Mx/Time/MxTime.h"

#include <iostream>
#include "Mx/Resource/Model/MxModel.h"
#include "Mx/Resource/Texture/MxTexture.h"
#include "Mx/Resource/Shader/MxShaderSource.h"
#include "Mx/Log/MxLog.h"
#include "Mx/Vulkan/MxVkGraphics.h"
#include "Mx/Resource/MxResourceLoader.h"
#include "Mx/Vulkan/Renderer/PresetRenderer/MxVkStandardRenderer.h"
#include "Mx/GUI/MxUi.h"

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
		delete mWindow;
		SDL_Quit();
	}

	int MixEngine::exec() {
		try {
			init();
			SDL_Event event;
			while (!mQuit) {
				while (SDL_PollEvent(&event)) {
					process(event);
				}
				update();
				render();
				lateUpdate();
				++mFrameCount;
				static auto lastTime = std::chrono::steady_clock::now();
				if (mFrameCount > 10) {
					auto now = std::chrono::steady_clock::now();
					float duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
					mFramePerSecond = static_cast<float>(mFrameCount) * 1000 / duration;
					lastTime = now;
					mFrameCount = 0;
				}
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

		//// todo test graphics
		mWindow = new Window("Surprise! Mother Fucker!", { 1200,800 }, SDL_WINDOW_VULKAN);
		auto instanceExtsReq = mWindow->getRequiredInstanceExts();
		instanceExtsReq.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		std::vector<const char*> deviceExtsReq;
		deviceExtsReq.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		std::vector<const char*> layersReq;
		layersReq.push_back("VK_LAYER_LUNARG_standard_validation");

		Graphics::VulkanSettings settings;
		settings.appInfo.appName = "Fuck";
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
		for (auto& ext : instanceExts) {
			Debug::Log::Info("--%s", ext.extensionName);
		}

		Debug::Log::Info("Validation Layers:");

		auto layers = Graphics::Vulkan::GetAllSupportedLayers();
		for (auto& layer : layers) {
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

		// add a model
		mCamera = new GameObject("Camera");
		mCamera->transform().setPosition(Math::Vector3f(0.0f, 0.0f, -1.0f));

		auto model = resourceLoader->load<Resource::Model>("E:/Git/vulkan-learning-master/res/models/gltfSample/DamagedHelmet/glTF/DamagedHelmet.gltf");
		mGameObject = model->generateGameObject();
		mGameObject->transform().setLocalScale(Math::Vector3f(0.5f, 0.5f, 0.5f));

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
			Input::mouseScrollDelta += Math::Vector2i(_event.wheel.x, deltaY);
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
		mWindow->setTitle(std::to_string(mFramePerSecond));
		auto tran = mCamera->getComponent<Mix::Transform>();
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_W)) {
			tran->translate(Math::Vector3f::Forward * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
			Debug::Log::Info("W");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_S)) {
			tran->translate(Math::Vector3f::Back * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
			Debug::Log::Info("S");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_D)) {
			tran->translate(Math::Vector3f::Right * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
			Debug::Log::Info("D");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_A)) {
			tran->translate(Math::Vector3f::Left * (Time::DeltaTime() * 5.0f), Mix::Space::SELF);
			Debug::Log::Info("A");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_SPACE)) {
			tran->translate(Math::Vector3f::Up * (Time::DeltaTime() * 5.0f), Mix::Space::WORLD);
			Debug::Log::Info("Space");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_LCTRL)) {
			tran->translate(Math::Vector3f::Down * (Time::DeltaTime() * 5.0f), Mix::Space::WORLD);
			Debug::Log::Info("Ctrl");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_Q)) {
			tran->rotate(Math::Vector3f::Up, Math::Radians(Time::DeltaTime()*-45.0f));
			Debug::Log::Info("Q");
		}
		if (Mix::Input::GetAxisRaw(SDL_SCANCODE_E)) {
			tran->rotate(Math::Vector3f::Up, Math::Radians(Time::DeltaTime()*45.0f));
			Debug::Log::Info("E");
		}

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
		getModule<Ui>()->update();
		getModule<Ui>()->render();
		getModule<Graphics::Vulkan>()->update();
		getModule<Graphics::Vulkan>()->render();
	}
}

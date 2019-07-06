#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Definitions/MxAudio.h"
#include "Mx/Scene/MxScene.h"

#include <fmod/fmod.hpp>
#include <SDL2/SDL.h>
#include "Mx/Engine/MxModuleHolder.h"

namespace Mix {
	class MixEngine : public GeneralBase::SingletonBase<MixEngine> {
		friend SingletonBase<MixEngine>;
		friend static FMOD::System* Audio::Core();
	public:
		~MixEngine();

		int exec();

		ModuleHolder& getModuleHolder() { return mModuleHolder; }

		template<typename _Ty>
		bool hasModule() const { return mModuleHolder.has<_Ty>(); }

		template<typename _Ty>
		_Ty* getModule() const { return mModuleHolder.get<_Ty>(); }

		template<typename _Ty, typename... _Args>
		_Ty* addModule(_Args&&... _args) { return mModuleHolder.add<_Ty>(std::forward<_Args>(_args)...); }

		template<typename _Ty>
		void removeModule() { mModuleHolder.remove<_Ty>(); };

		const Window& getWindow() const { return *mWindow; }
		//static MixEngine& Instance(int _argc = 0, char** _argv = nullptr) {
		//    static MixEngine instance(_argc, _argv);
		//    return instance;
		//}
	private:

		// todo: delete debug code
		// Scene mScene;

		explicit MixEngine(int _argc = 0, char** _argv = nullptr);

		bool mQuit;
		FMOD::System* mFmodCore;

		void init();
		void process(const SDL_Event& _event);
		void update();
		void fixedUpdate();
		void lateUpdate();
		void render();

		Window* mWindow = nullptr;
		GameObject* mCamera = nullptr;
		GameObject* mGameObject = nullptr;

		uint32_t mFrameCount = 0;
		float mFramePerSecond = 0;

		ModuleHolder mModuleHolder;
	};
}

#endif

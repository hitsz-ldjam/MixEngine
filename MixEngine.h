#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Engine/MxModuleHolder.h"
#include "Mx/Scene/MxScene.h"
#include <SDL2/SDL_events.h>

namespace Mix {
    class Window;

    class MixEngine : public GeneralBase::SingletonBase<MixEngine> {
        friend SingletonBase<MixEngine>;

    public:
        ~MixEngine();

        int exec();

        bool& quit() noexcept { return mQuit; }

        Window& getWindow() { return *mWindow; }

        // ----- ModuleHolder -----

        template<typename _Ty>
        bool hasModule() const { return mModuleHolder.has<_Ty>(); }

        template<typename _Ty>
        _Ty* getModule() const { return mModuleHolder.get<_Ty>(); }

        template<typename _Ty, typename... _Args>
        _Ty* addModule(_Args&&... _args) { return mModuleHolder.add<_Ty>(std::forward<_Args>(_args)...); }

        template<typename _Ty>
        void removeModule() { mModuleHolder.remove<_Ty>(); }

        ModuleHolder& getModuleHolder() { return mModuleHolder; }

    private:
        explicit MixEngine(int _argc = 0, char** _argv = nullptr);

        bool mQuit;

        Window* mWindow;
        ModuleHolder mModuleHolder;

        // todo: debug code
        Scene mDebugScene;

        // todo: make this a utility class
        uint32_t mFrameCount = 0u,
                 mFrameSampleRate = 10u;
        float mFramePerSecond = .0f;

        void awake();
        void init();
        void process(const SDL_Event& _event);
        void update();
        void fixedUpdate();
        void lateUpdate();
        void render();
    };
}

#endif

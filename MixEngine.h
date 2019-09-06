#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Engine/MxModuleHolder.h"

namespace Mix {
    class Window;

    class MixEngine : public GeneralBase::SingletonBase<MixEngine> {
        friend SingletonBase<MixEngine>;

    public:
        ~MixEngine();

        int exec();

        void shutDown() { mQuit = true; }

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

        ModuleHolder mModuleHolder;

        // todo: make this a utility class
        uint32_t mFrameCount = 0u,
                 mFrameSampleRate = 10u;
        float mFramePerSecond = .0f;

        void awake();
        void init();
        void update();
        void fixedUpdate();
        void lateUpdate();
        void render();
        void postRender();
    };
}

#endif

#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Engine/MxModuleHolder.h"
#include "Mx/Utils/MxEvent.h"

namespace Mix {
    class Window;
    class ApplicationBase;

    class MixEngine : public GeneralBase::SingletonBase<MixEngine> {
        friend SingletonBase<MixEngine>;
    public:
        ~MixEngine();

        int execute(std::shared_ptr<ApplicationBase> _app);

        /**
         * \brief Call to issue a request for the application to close. \n
         *        This will eventually trigger an quit event and onQuitRequested() will be called.
         * \note  ONLY call this after startUp() has been called
         */
        void requestQuit();

    private:
        explicit MixEngine(int _argc = 0, char** _argv = nullptr);

        void quit() { mQuit = true; mRunning = false; }

        bool mRunning = true;
        bool mQuit = false;

        //////////////////////////////////////////////////////////////////
        //                              FPS                             //
        //////////////////////////////////////////////////////////////////

    public:
        void setFPSLimit(uint32_t _limit);

        float getFPS() const { return mFramePerSecond; }

    private:
        uint32_t mFPSLimit = 0;
        uint32_t mFrameCount = 0;
        uint32_t mFrameSampleRate = 5;
        float mFramePerSecond = 0.0f;
        float mLastFrameTime = 0.0f;
        float mFrameStep = 0.0f;

        //////////////////////////////////////////////////////////////////
        //                         Setup scene                          //
        //////////////////////////////////////////////////////////////////
        void loadMainScene();


        //////////////////////////////////////////////////////////////////
        //                          Main loop                           //
        //////////////////////////////////////////////////////////////////
    private:
        /*void awake();
        void init();*/
        void update();
        void fixedUpdate();
        void lateUpdate();

        //////////////////////////////////////////////////////////////////
        //                           Render                             //
        //////////////////////////////////////////////////////////////////
    private:
        void render();
        void postRender();

        //////////////////////////////////////////////////////////////////
        //                           Modules                            //
        //////////////////////////////////////////////////////////////////

    public:
        void loadModule();
        void initModule();

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
        std::shared_ptr<ApplicationBase> mApp;
        ModuleHolder mModuleHolder;

        //////////////////////////////////////////////////////////////////
        //                           Callbacks                          //
        //////////////////////////////////////////////////////////////////
        void onQuitRequested();


        //////////////////////////////////////////////////////////////////
        //                            Events                            //
        //////////////////////////////////////////////////////////////////

    /*public:
        Event<void()> ModuleLoadedEvent;
        Event<void()> MoudleInitializedEvent;
        Event<void()> MainSceneCreatedEvent;
        Event<void()> AwakeEvent;
        Event<void()> InitEvent;
        Event<void()> UpdateEvent;
        Event<void()> FixedUpdateEvent;
        Event<void()> LateUpdateEvent;
        Event<void()> RenderEvent;
        Event<void()> PostRenderEvent;*/


        //////////////////////////////////////////////////////////////////
        //                        Command lines                         //
        //////////////////////////////////////////////////////////////////
    public:
        const std::vector<std::string>& getCommandLines() const { return mCommandLines; }

    private:
        std::vector<std::string> mCommandLines;
    };
}

#endif

#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Definitions/MxAudio.h"
#include "Mx/Scene/MxScene.h"

#include <fmod/fmod.hpp>
#include <SDL2/SDL.h>
#include "Mx/Vulkan/MxVkGraphics.h"
#include "Mx/Resource/MxResource.h"

namespace Mix {
    class MixEngine {
        friend static FMOD::System* Audio::Core();

    public:
        ~MixEngine();
        int exec();

        static MixEngine& Instance(int _argc = 0, char** _argv = nullptr) {
            static MixEngine instance(_argc, _argv);
            return instance;
        }

        MixEngine(const MixEngine&) = delete;
        void operator=(const MixEngine&) = delete;

    private:

        // todo: delete debug code
        // Scene mScene;

        MixEngine(int _argc = 0, char** _argv = nullptr);

        bool mQuit;
        FMOD::System* mFmodCore;

        void init();
        void process(const SDL_Event& _event);
        void update();
        void fixedUpdate();
        void lateUpdate();
        void render();

        // todo test add graphics here
        Window* mWindow = nullptr;
        Graphics::Vulkan* mVulkan = nullptr;
        Resource::ResourceLoader* mResources = nullptr;
        GameObject* mCamera = nullptr;
        GameObject* mGameObject = nullptr;
    };
}

#endif

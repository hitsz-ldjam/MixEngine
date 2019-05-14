#pragma once

#ifndef MIX_ENGINE_H_
#define MIX_ENGINE_H_

#include "Mx/Definitions/MxAudio.h"
#include "Mx/Scene/MxScene.h"

#include <fmod/fmod.hpp>
#include <SDL2/SDL.h>

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
        Scene mScene;

        // todo: replace with new timing interface
        std::chrono::time_point<std::chrono::high_resolution_clock> start, lastFrame;

        MixEngine(int _argc = 0, char** _argv = nullptr);

        bool mQuit;
        FMOD::System* mFmodCore;

        void init();
        void process(const SDL_Event& _event);
        void update();
        void lateUpdate();
        void render();
    };
}

#endif

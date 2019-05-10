#pragma once

#ifndef _MIX_ENGINE_H
#define _MIX_ENGINE_H

#include <iostream>

#include <SDL2/SDL.h>

#include "Mx/Input/MxInput.h"
#include "Mx/Hierarchy/MxHierarchy.h"
#include "Mx/Window/MxWindow.h"

// todo: replace with new timing interface
#include "Mx/Time/MxTime.h"

namespace Mix {
    class MixEngine {
    public:
        MixEngine(int argc = 0, char** argv = nullptr);
        ~MixEngine();
        int exec();

    private:
        bool quit;

        // todo: delete debug code
        Hierarchy hierarchy;

        // todo: replace with new timing interface
        std::chrono::time_point<std::chrono::high_resolution_clock> start, lastFrame;

        void init();
        void process(const SDL_Event& event);
        void update();
        void lateUpdate();
        void render();
    };
}

#endif

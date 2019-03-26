#pragma once

#ifndef _MX_APPLICATION_H_
#define _MX_APPLICATION_H_

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "MxAudioManager.h"
#include "MxBehaviour.h"
#include "MxInput.h"
#include "MxTime.h"

namespace Mix {
    class Application {
    public:
        std::vector<Behaviour*> behaviours;
        Application(int argc = 0, char** argv = nullptr);
        ~Application();
        int exec();

    private:
        bool quit;
        SDL_Event event;
        std::chrono::time_point<std::chrono::high_resolution_clock> start, lastFrame;

        // todo: where to insert?
        void fixedUpdate() { for(auto be : behaviours) be->fixedUpdate(); }
        void init();
        void lateUpdate();
        void process(SDL_Event& event);
        void render();
        void update() { for(auto be : behaviours) be->update(); }

        void postRender();
        void preEvent();
        void preRender();
    };
}

#endif

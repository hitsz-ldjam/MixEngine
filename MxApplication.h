#pragma once

#ifndef _MX_APPLICATION_H_
#define _MX_APPLICATION_H_

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "MxBehaviour.h"
#include "MxInput.h"

namespace Mix {
    class Application {
    public:
        std::vector<Behaviour*> behaviours;
        Application(int argc = 0, char** argv = nullptr);
        ~Application();
        int exec();

    private:
        bool quit;

        void cleanup();
        // todo: where to insert?
        void fixedUpdate() { for(auto be : behaviours) be->fixedUpdate(); }
        void init();
        void lateUpdate() { for(auto be : behaviours) be->lateUpdate(); }
        void process(SDL_Event& event);
        void render();
        void update() { for(auto be : behaviours) be->update(); }
    };
}

#endif

#pragma once

#ifndef _MX_APPLICATION_H_
#define _MX_APPLICATION_H_

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "MxInput.h"

// todo: delete test code
#include "MxWindow.h"

namespace Mix {
    class Application {
    public:
        // std::vector<std::shared_ptr<Behaviour>> behaviours;
        Application(int argc = 0, char** argv = nullptr);
        ~Application();
        int exec();
    private:
        bool _quit;
        void init();
        void preProcess();
        void process(SDL_Event& event);
        void update();
        void render();
        void quit();
        void cleanup();
    };
}

#endif

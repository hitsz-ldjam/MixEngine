#pragma once

#ifndef _MX_WINDOW_H_
#define _MX_WINDOW_H_

#include <stdexcept>
#include <string>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>

namespace Mix {
    class Window {
    public:
        Window();
        Window(SDL_Window* window);
        Window(const std::string& title, const int width, const int height, const Uint32 flags = 0);
        ~Window();
        void create(const std::string& title, const int width, const int height, const Uint32 flags = 0);
        void setIcon(const std::string& filename);
        void destory();
        SDL_Window* getWindowPtr();
    private:
        SDL_Window* window;
    };
}

#endif

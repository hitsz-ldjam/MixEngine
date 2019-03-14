#pragma once

#ifndef _MX_WINDOW_H_
#define _MX_WINDOW_H_

#include <stdexcept>
#include <string>

#include <glm/vec2.hpp>

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
        /**
         * @note The window size in screen coordinates may differ from the size in pixels,
         * if the window was created with SDL_WINDOW_ALLOW_HIGHDPI on a high-DPI supported platform (eg. iOS and MAC OS X).
         * However, I can't afford such devices so I don't care.
         */
        glm::ivec2 getSize();
        void destory();
        SDL_Window* getWindowPtr();
    private:
        SDL_Window* window;
    };
}

#endif

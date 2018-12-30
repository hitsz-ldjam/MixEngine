#pragma once

#ifndef _MX_WINDOW_H_
#define _MX_WINDOW_H_

#include <stdexcept>
#include <string>

#include <SDL.h>

namespace Mixel {
    struct MxRect { int width, height; };

    class MxWindow {
    public:
        MxWindow();
        ~MxWindow();
        void create(const std::string title, const MxRect rect, const unsigned int flag = 0);
        void create(const std::string title, const int width, const int height, const unsigned int flag = 0);
        void destory();
        SDL_Window* getWindow();
        const std::string getTitle();
        const MxRect getWindowRect();
    private:
        SDL_Window* window;
        std::string title;
        MxRect rect;
    };
}

#endif

#pragma once

#ifndef _MX_WINDOW_H_
#define _MX_WINDOW_H_

#include <stdexcept>
#include <string>

#include <SDL_video.h>

#include "MxImage.h"

namespace Mix {
    class MxWindow {
    public:
        MxWindow();
        MxWindow(const std::string title, const int width, const int height, const unsigned int flags = 0);
        ~MxWindow();
        void create(const std::string title, const int width, const int height, const unsigned int flags = 0);
        void setIcon(const std::string filename);
        void destory();
        SDL_Window* getWindowPtr();
    private:
        SDL_Window* window;
    };
}

#endif

#include "MxWindow.h"

namespace Mix {
    MxWindow::MxWindow() {
        window = nullptr;
    }

    MxWindow::MxWindow(const std::string title, const int width, const int height, const unsigned int flags) {
        MxWindow::create(title, width, height, flags);
    }

    MxWindow::~MxWindow() {
        MxWindow::destory();
    }

    void MxWindow::create(const std::string title, const int width, const int height, const unsigned int flags) {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                                  flags | SDL_WINDOW_SHOWN);
        //                        flags | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        if(!window)
            throw std::runtime_error("Error: Failed to create window");
    }

    void MxWindow::setIcon(const std::string filename) {
        if(!window) return;
        MxImage icon(filename);
        SDL_SetWindowIcon(window, icon.getSurfacePtr());
    }

    void MxWindow::destory() {
        if(window)
            SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Window* MxWindow::getWindowPtr() {
        return window;
    }
}

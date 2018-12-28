#include "MXWindow.h"

namespace Mixel {
    MXWindow::MXWindow() {
        window = nullptr;
    }

    MXWindow::~MXWindow() {
        if(window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    void MXWindow::create(const std::string title, const int width, const int height) {
        // Edit flag in demo -> SDL_WINDOW_VULKAN
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN);
        this->title = title;
        rect = {width, height};

        if(!window) {
            throw std::runtime_error("Error: Failed to create window");
        }
    }

    void MXWindow::destory() {
        if(window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    const SDL_Window* MXWindow::getWindow() {
        return window;
    }

    const std::string MXWindow::getTitle() {
        return title;
    }

    const MXRect MXWindow::getWindowRect() {
        return rect;
    }

}

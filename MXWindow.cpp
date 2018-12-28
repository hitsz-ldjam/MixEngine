#include "MXWindow.h"

namespace Mixel {
    MXWindow::MXWindow() {
        window = nullptr;
    }

    MXWindow::~MXWindow() {
        MXWindow::destory();
    }

    void MXWindow::create(const std::string title, const MXRect rect, const unsigned int flag) {
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  rect.width, rect.height, flag | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        this->title = title;
        this->rect = rect;

        if(!window) {
            throw std::runtime_error("Error: Failed to create window");
        }
    }

    void MXWindow::create(const std::string title, const int width, const int height, const unsigned int flag) {
        MXWindow::create(title, {width, height}, flag);
    }

    void MXWindow::destory() {
        if(window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    SDL_Window* MXWindow::getWindow() {
        return window;
    }

    const std::string MXWindow::getTitle() {
        return title;
    }

    const MXRect MXWindow::getWindowRect() {
        return rect;
    }
}

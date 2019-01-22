#include "MxWindow.h"

namespace Mix {
    MxWindow::MxWindow() {
        window = nullptr;
    }

    MxWindow::MxWindow(const std::string title, const MxRect rect, const unsigned int flag) {
        MxWindow::create(title, rect, flag);
    }

    MxWindow::MxWindow(const std::string title, const int width, const int height, const unsigned int flag) {
        MxWindow::create(title, width, height, flag);
    }

    MxWindow::~MxWindow() {
        MxWindow::destory();
    }

    void MxWindow::create(const std::string title, const MxRect rect, const unsigned int flag) {
        //window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        //                          rect.width, rect.height, flag | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  rect.width, rect.height, flag | SDL_WINDOW_SHOWN);
        this->title = title;
        this->rect = rect;

        if(!window) {
            throw std::runtime_error("Error: Failed to create window");
        }
    }

    void MxWindow::create(const std::string title, const int width, const int height, const unsigned int flag) {
        MxWindow::create(title, {width, height}, flag);
    }

    void MxWindow::destory() {
        if(window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }

    SDL_Window* MxWindow::getWindowPtr() {
        return window;
    }

    const std::string MxWindow::getTitle() {
        return title;
    }

    const MxRect MxWindow::getWindowRect() {
        return rect;
    }
}

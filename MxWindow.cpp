#include "MxWindow.h"

namespace Mix {
    Window::Window() : window(nullptr) {}

    Window::Window(SDL_Window* window) : window(window) {}

    Window::Window(const std::string& title, const int width, const int height, const Uint32 flags) {
        window = nullptr;
        create(title, width, height, flags);
    }

    Window::~Window() {
        destory();
    }

    void Window::create(const std::string& title, const int width, const int height, const Uint32 flags) {
        if(window)
            return;
        window = SDL_CreateWindow(title.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width,
                                  height,
                                  flags | SDL_WINDOW_SHOWN);
        //                        flags | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        if(!window)
            throw std::runtime_error("Error: Failed to create window");
    }

    void Window::setIcon(Surface icon) {
        SDL_SetWindowIcon(window, icon.getSurfacePtr());
    }

    void Window::destory() {
        if(window)
            SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Window* Window::getWindowPtr() {
        return window;
    }
}

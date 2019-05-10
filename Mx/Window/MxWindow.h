#pragma once

#ifndef MX_WINDOW_H
#define MX_WINDOW_H

#include <string>
#include <filesystem>

#include <glm/vec2.hpp>
#include <SDL2/SDL.h>
#include <stb_image/stb_image.h>

#include "../Exceptions/MxExceptions.hpp"

namespace Mix {
    class Window {
    public:
        Window(SDL_Window* _window = nullptr) : mWindow(_window) {}
        Window(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags = 0);
        ~Window();

        void create(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags = 0);
        void setIcon(const std::filesystem::path& _path);

        glm::ivec2 drawableSize();

        SDL_Window* window() { return mWindow; }
        SDL_Surface* surface() { return SDL_GetWindowSurface(mWindow); }

        static bool GetRelativeMouseMode() {
            return SDL_GetRelativeMouseMode();
        }

        static void SetRelativeMouseMode(const bool _enable) {
            SDL_SetRelativeMouseMode(_enable ? SDL_TRUE : SDL_FALSE);
        }

    private:
        SDL_Window* mWindow;
    };
}

#endif

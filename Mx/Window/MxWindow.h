#pragma once

#ifndef MX_WINDOW_H_
#define MX_WINDOW_H_

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>

#include <filesystem>
#include <string>

namespace Mix {
    class Window final {
    public:
        Window(SDL_Window* _window = nullptr) : mWindow(_window) {}
        Window(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags = 0);
        ~Window();

        void create(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags = 0);
        void setIcon(const std::filesystem::path& _path);

        /**
         *  @note This function calls
         */
        glm::ivec2 drawableSize() const;

        SDL_Window* window() const { return mWindow; }
        SDL_Surface* surface() const { return SDL_GetWindowSurface(mWindow); }

        static bool GetRelativeMouseMode() {
            return SDL_GetRelativeMouseMode() == SDL_TRUE;
        }

        static void SetRelativeMouseMode(const bool _enable) {
            SDL_SetRelativeMouseMode(_enable ? SDL_TRUE : SDL_FALSE);
        }

    private:
        SDL_Window* mWindow;
    };
}

#endif

#pragma once

#ifndef MX_WINDOW_H_
#define MX_WINDOW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
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

        glm::ivec2 drawableSize() const;

        SDL_Window* window() const { return mWindow; }
        SDL_Surface* surface() const { return SDL_GetWindowSurface(mWindow); }

        std::string getTitle() const {
            std::string str = "";
            if(mWindow) {
                const char* c = SDL_GetWindowTitle(mWindow);
                str = c;
            }
            return str;
        }

        void setTitle(const std::string& _title) const {
            if(mWindow)
                SDL_SetWindowTitle(mWindow, _title.c_str());
        }

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

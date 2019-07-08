#pragma once

#ifndef MX_WINDOW_H_
#define MX_WINDOW_H_

#include "../Math/MxVector2.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <filesystem>
#include <string>

namespace Mix {
    class Window final {
    public:
        explicit Window(SDL_Window* _window = nullptr) : mWindow(_window) {}

        Window(const std::string& _title, const Math::Vector2i& _size, const Uint32 _flags = 0);

        ~Window();

        void create(const std::string& _title, const Math::Vector2i& _size, const Uint32 _flags = 0);

        void setIcon(const std::filesystem::path& _path);

        Math::Vector2i drawableSize() const;

        Math::Vector2i extent() const;

        SDL_Window* window() const { return mWindow; }

        SDL_Surface* surface() const { return SDL_GetWindowSurface(mWindow); }

        std::string getTitle() const {
            if(mWindow)
                return SDL_GetWindowTitle(mWindow);
            return "";
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

        auto getRequiredInstanceExts() const {
            unsigned int count;
            SDL_Vulkan_GetInstanceExtensions(mWindow, &count, nullptr);
            std::vector<const char*> result(count);
            SDL_Vulkan_GetInstanceExtensions(mWindow, &count, result.data());
            return result;
        }

    private:
        SDL_Window* mWindow;
    };
}

#endif

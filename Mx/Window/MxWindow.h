#pragma once

#ifndef MX_WINDOW_H_
#define MX_WINDOW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include "../Math/MxVector2.h"

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

		std::vector<const char*> getRequiredInstanceExts() const {
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

#include "MxWindow.h"
#include "../Exceptions/MxExceptions.hpp"
#include <stb_image/stb_image.h>

namespace Mix {
    Window::Window(const std::string& _title, const Math::Vector2i& _size, const Uint32 _flags) {
        mWindow = nullptr;
        create(_title, _size, _flags);
    }

    Window::~Window() {
        if(mWindow)
            SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    void Window::create(const std::string& _title, const Math::Vector2i& _size, const Uint32 _flags) {
        if(mWindow)
            return;
        mWindow = SDL_CreateWindow(_title.c_str(),
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   _size.x,
                                   _size.y,
                                   _flags | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
        if(!mWindow)
            throw WindowCreationError();
    }

    void Window::setIcon(const std::filesystem::path& _path) {
        const int format = STBI_rgb_alpha;
        int channels;
        glm::ivec2 size;
        auto data = static_cast<unsigned char*>(stbi_load(_path.string().c_str(),
                                                          &size.x,
                                                          &size.y,
                                                          &channels,
                                                          format));
        if(!data)
            throw WindowIconLoadingError();

        SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(data,
                                                               size.x,
                                                               size.y,
                                                               32,
                                                               4 * size.x,
                                                               SDL_PIXELFORMAT_RGBA32);
        if(!icon) {
            stbi_image_free(data);
            throw WindowIconLoadingError();
        }

        SDL_SetWindowIcon(mWindow, icon);
        SDL_FreeSurface(icon);
        stbi_image_free(data);
    }

    Math::Vector2i Window::drawableSize() const {
        Math::Vector2i size;
        if(mWindow)
            SDL_Vulkan_GetDrawableSize(mWindow, &size.x, &size.y);
        return size;
    }

    Math::Vector2i Window::extent() const {
        Math::Vector2i size;
        if(mWindow)
            SDL_GetWindowSize(mWindow, &size.x, &size.y);
        return size;
    }
}

#include "MxWindow.h"

#include "../Exceptions/MxExceptions.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Mix {
    Window::Window(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags) {
        mWindow = nullptr;
        create(_title, _size, _flags);
    }

    Window::~Window() {
        if(mWindow)
            SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    void Window::create(const std::string& _title, const glm::ivec2& _size, const Uint32 _flags) {
        if(mWindow)
            return;
        mWindow = SDL_CreateWindow(_title.c_str(),
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   _size.x,
                                   _size.y,
                                   _flags | SDL_WINDOW_SHOWN /*| SDL_WINDOW_VULKAN*/);
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

    glm::ivec2 Window::drawableSize() const {
        glm::ivec2 size = {0, 0};
        if(mWindow)
            SDL_GL_GetDrawableSize(mWindow, &size.x, &size.y);
        return size;
    }
}

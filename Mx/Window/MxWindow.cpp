#include "MxWindow.h"

namespace Mix {
    Window::Window(const std::string& _title, const glm::ivec2& _size, const uint32_t _flags) {
        mWindow = nullptr;
        create(_title, _size, _flags);
    }

    Window::~Window() {
        if(mWindow)
            SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    void Window::create(const std::string& _title, const glm::ivec2& _size, const uint32_t _flags) {
        if(mWindow)
            return;
        mWindow = SDL_CreateWindow(_title.c_str(),
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   _size.x,
                                   _size.y,
                                   static_cast<Uint32>(_flags) | SDL_WINDOW_SHOWN /*| SDL_WINDOW_VULKAN*/);
        if(!mWindow)
            throw std::runtime_error("[ERROR] Failed to create window");
    }

    void Window::setIcon(const std::filesystem::path& _path) {
        const int format = STBI_rgb_alpha;
        glm::ivec3 info;
        auto data = static_cast<unsigned char*>(stbi_load(_path.string().c_str(),
                                                          &info.x,
                                                          &info.y,
                                                          &info.z,
                                                          format));
        if(!data)
            throw std::runtime_error("[ERROR] Failed to load icon image");

        SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(data,
                                                               info.x,
                                                               info.y,
                                                               32,
                                                               4 * info.x,
                                                               SDL_PIXELFORMAT_RGBA32);
        if(!icon) {
            stbi_image_free(data);
            throw std::runtime_error("[ERROR] Failed to create icon surface");
        }

        SDL_SetWindowIcon(mWindow, icon);
        SDL_FreeSurface(icon);
        stbi_image_free(data);
    }

    glm::ivec2 Window::drawableSize() {
        glm::ivec2 size = {0, 0};
        if(mWindow)
            SDL_GL_GetDrawableSize(mWindow, &size.x, &size.y);
        return size;
    }
}

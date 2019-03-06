#pragma once

#ifndef _MX_IMAGE_H_
#define _MX_IMAGE_H_

#include <stdexcept>
#include <string>

#include <SDL2/SDL_image.h>

namespace Mix {
    class Surface {
    public:
        Surface();
        Surface(const std::string& filename);
        ~Surface();
        void loadFromFile(const std::string& filename);
        void free();
        SDL_Surface* getSurfacePtr();
    private:
        SDL_Surface* surface;
    };
}

#endif

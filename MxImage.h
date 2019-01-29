#pragma once

#ifndef _MX_IMAGE_H_
#define _MX_IMAGE_H_

#include <stdexcept>
#include <string>

#include <SDL_image.h>

namespace Mix {
    class MxImage {
    public:
        MxImage();
        MxImage(std::string filename);
        ~MxImage();
        void load(std::string filename);
        void free();
        SDL_Surface* getSurfacePtr();
    private:
        SDL_Surface* image;
    };
}

#endif

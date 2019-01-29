#include "MxImage.h"

namespace Mix {
    MxImage::MxImage() {
        image = nullptr;
    }

    MxImage::MxImage(std::string filename) {
        MxImage::load(filename);
    }

    MxImage::~MxImage() {
        MxImage::free();
    }

    void MxImage::load(std::string filename) {
        image = IMG_Load(filename.c_str());
        if(!image)
            throw std::runtime_error("Error: Cannot load image " + filename);
    }

    void MxImage::free() {
        if(image)
            SDL_FreeSurface(image);
        image = nullptr;
    }

    SDL_Surface* MxImage::getSurfacePtr() {
        return image;
    }
}

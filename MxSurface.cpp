#include "MxSurface.h"

namespace Mix {
    Surface::Surface() {
        surface = nullptr;
    }

    Surface::Surface(const std::string& filename) {
        surface = nullptr;
        loadFromFile(filename);
    }

    Surface::~Surface() {
        free();
    }

    void Surface::loadFromFile(const std::string& filename) {
        if(surface)
            return;
        surface = IMG_Load(filename.c_str());
        if(!surface)
            throw std::runtime_error("Error: Cannot load surface " + filename);
    }

    void Surface::free() {
        if(surface)
            SDL_FreeSurface(surface);
        surface = nullptr;
    }

    SDL_Surface* Surface::getSurfacePtr() {
        return surface;
    }
}

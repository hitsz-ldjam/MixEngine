#include "Demo.h"

Demo::Demo() {}

Demo::~Demo() {
    puts("Demo deleted");
}

void Demo::init() {
    window.create("Keyborad events test", 640, 480);
    // window.setIcon(icon_path_here);
    SDL_Surface* surface = SDL_GetWindowSurface(window.getWindowPtr());
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 128, 225));
    SDL_UpdateWindowSurface(window.getWindowPtr());
}

void Demo::update() {}

void Demo::onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID) {
    std::cout << scancode << std::endl;
}

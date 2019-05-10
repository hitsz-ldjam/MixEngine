#include "MxHierarchy.h"

namespace Mix {
    void Hierarchy::init() {
        fuckGarbageCPPWindow.create("Mx unit test", {800, 600});
        SDL_FillRect(fuckGarbageCPPWindow.surface(), nullptr, SDL_MapRGB(fuckGarbageCPPWindow.surface()->format, 0, 128, 225));
        SDL_UpdateWindowSurface(fuckGarbageCPPWindow.window());
    }

    void Hierarchy::update() {
        /*glm::ivec3 pos = Input::MousePosition();
        std::cout << pos.x << ", " << pos.y << ", " << (pos.z & SDL_BUTTON_LMASK) << ", " << ", " << std::endl;*/
    }
}

#include "MxScene.h"

namespace Mix {
    Scene::Scene() : window(nullptr) {}

    Scene::~Scene() {
        delete window;
    }

    void Scene::init() {
        window->create("Mx unit test", {800, 600});
        SDL_FillRect(window->surface(), nullptr, SDL_MapRGB(window->surface()->format, 0, 128, 225));
        SDL_UpdateWindowSurface(window->window());
    }

    void Scene::update() {
        /*glm::ivec3 pos = Input::MousePosition();
        std::cout << pos.x << ", " << pos.y << ", " << (pos.z & SDL_BUTTON_LMASK) << ", " << ", " << std::endl;*/
    }

    void Scene::fixedUpate() {}

    void Scene::lateUpate() {}
}

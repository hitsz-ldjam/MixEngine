#include "MxScene.h"
#include <glm/vec3.hpp>
#include "../Input/MxInput.h"
#include <iostream>

namespace Mix {
    Scene::~Scene() {
        delete mWindow;
    }

    void Scene::init() {
        mWindow = new Window();
        mWindow->create("Mx unit test", { 800,600 });
        SDL_FillRect(mWindow->surface(), nullptr, SDL_MapRGB(mWindow->surface()->format, 0, 128, 225));
        SDL_UpdateWindowSurface(mWindow->window());
    }

    void Scene::update() {
        /*glm::ivec2 pos = Input::MousePositionDelta();
        std::cout << pos.x << ", " << pos.y << std::endl;*/
    }

    void Scene::fixedUpate() {}

    void Scene::lateUpate() {}

    Scene::Scene(const std::string &_str) {
        mName = _str;
        mIsLoaded = false;
        mBuildIndex = 0;
    }
    std::string Scene::getName() const {
        return mName;
    }

    void Scene::addGameObject(GameObject* _obj) {
        mHierarchy.push_back(_obj);
    }
    void Scene::removeGameObject(GameObject* _obj) {
        std::vector<GameObject*>::iterator iter = std::find(mHierarchy.begin(), mHierarchy.end(), _obj);
        mHierarchy.erase(iter);
    }
    void Scene::getLoaded() {
        mIsLoaded = true;
    }
    bool Scene::isLoad() {
        return mIsLoaded;
    }


    std::vector<GameObject*> Scene::getRootGameObject() {
        return mHierarchy;
    }
    size_t Scene::buildIndex() const {
        return mBuildIndex;
    }
}

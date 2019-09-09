#include "MxScene.h"
#include "../GameObject/MxGameObject.h"
#include "../BuildIn/Camera/MxCamera.h"

namespace Mix {
    const std::string Scene::defaultMainSceneName = "MainScene";
    const HierarchyLoader Scene::defaultMainSceneOnLoad = []() { return Hierarchy(); };

    GameObject* Scene::findGameObject(const std::string& _name) const {
        return findGameObjectIf([&name = _name](GameObject* _obj) { return _obj->getName() == name; });
    }

    Camera* Scene::mainCamera() const {
        auto obj = findGameObjectIf([](GameObject* _obj) { return _obj->getComponent<Camera>(); });
        return obj ? obj->getComponent<Camera>() : nullptr;
    }

    void Scene::load() {
        if(mIsLoaded)
            unload();
        mRootObjs = mOnLoad();
        for(auto obj : mRootObjs)
            obj->addToScene(this);
        mIsLoaded = true;
    }

    void Scene::unload() {
        if(!mIsLoaded) return;
        for(auto obj : mRootObjs) {
            obj->removeFromScene(this);
            delete obj;
        }
        mRootObjs.clear();
        mIsLoaded = false;
    }

    void Scene::awake() {
        for(auto obj : mRootObjs)
            obj->awake();
    }

    void Scene::init() {
        for(auto obj : mRootObjs)
            obj->init();
    }

    void Scene::update() {
        for(auto obj : mRootObjs)
            obj->update();
    }

    void Scene::fixedUpate() {
        for(auto obj : mRootObjs)
            obj->fixedUpdate();
    }

    void Scene::lateUpate() {
        for(auto obj : mRootObjs)
            obj->lateUpdate();
    }
}

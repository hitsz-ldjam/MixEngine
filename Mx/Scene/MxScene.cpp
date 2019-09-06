#include "MxScene.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
    const std::string Scene::defaultMainSceneName = "MainScene";
    const HierarchyLoader Scene::defaultMainSceneOnLoad = []() { return Hierarchy(); };

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

#include "MxScene.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
    Scene::Scene(std::string _name, const size_t _buildIdx) : mName(std::move(_name)),
                                                              mBuildIndex(_buildIdx),
                                                              mIsLoaded(false) {
        // todo: manage Scene by SceneMgr
        mIsLoaded = true;
    }

    void Scene::awake() const {
        for(auto obj : mHierarchy)
            obj->awake();
    }

    void Scene::init() const {
        for(auto obj : mHierarchy)
            obj->init();
    }

    void Scene::update() const {
        for(auto obj : mHierarchy)
            obj->update();
    }

    void Scene::fixedUpate() const {
        for(auto obj : mHierarchy)
            obj->fixedUpdate();
    }

    void Scene::lateUpate() const {
        for(auto obj : mHierarchy)
            obj->lateUpdate();
    }
}

#include "MxSceneManagerFactory.h"
#include "MxScene.h"

namespace Mix {
    SceneManagerFactory::~SceneManagerFactory() {
        for(auto scene : mScenes)
            delete scene;
    }

    //Scene& SceneManagerFactory::createScene(const std::string& _name) {
    //    auto ptr = new Scene(_name, mScenes.size());
    //    mScenes.push_back(ptr);
    //    return *ptr;
    //}

    //Scene* SceneManagerFactory::getActiveScene() const {
    //    return mIsActive;
    //}


    //void SceneManagerFactory::loadScene(size_t _sceneBuildIndex) {
    //    auto iter = mScenes.begin();
    //    while((*iter)->buildIndex() != _sceneBuildIndex) {
    //        ++iter;
    //    }
    //    (*iter)->getLoaded();
    //}

    //void SceneManagerFactory::loadScene(const std::string& _scenename) {
    //    auto iter = mScenes.begin();
    //    while((*iter)->getName() != _scenename) {
    //        ++iter;
    //    }
    //    (*iter)->getLoaded();
    //}

    //bool SceneManagerFactory::makeSceneActive(Scene* s) {
    //    return (mIsActive = s);
    //}
}

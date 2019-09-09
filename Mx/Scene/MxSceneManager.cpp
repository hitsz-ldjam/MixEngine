#include "MxSceneManager.h"

#include "../../Scenes/MainScene.hpp"

namespace Mix {
    SceneManager::SceneManager() : mActiveScene(new Scene(ScenesDebug::mainSceneName,
                                                          ScenesDebug::mainSceneOnLoad,
                                                          0)) {
        mScenes.push_back(mActiveScene);
        mActiveScene->load();
    }

    SceneManager::~SceneManager() {
        for(auto s : mScenes) {
            s->unload();
            delete s;
        }
    }

    Scene* SceneManager::createScene(std::string _name, HierarchyLoader _onLoad) {
        auto s = new Scene(std::move(_name), std::move(_onLoad), mScenes.size());
        mScenes.push_back(s);
        return s;
    }

    Scene* SceneManager::loadScene(const std::string& _name) const {
        auto iter = std::find_if(mScenes.cbegin(), mScenes.cend(), [&name = _name](auto* _s) { return _s->name() == name; });
        if(iter == mScenes.cend())
            return nullptr;
        if(mActiveScene) mActiveScene->unload();
        (*iter)->load();
        return *iter;
    }

    Scene* SceneManager::loadScene(const size_t _buildIdx) const {
        if(_buildIdx >= mScenes.size())
            return nullptr;
        if(mActiveScene) mActiveScene->unload();
        auto s = mScenes[_buildIdx];
        s->load();
        return s;
    }
}

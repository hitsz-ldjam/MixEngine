#include"MxSceneManager.h"

namespace Mix {
    size_t SceneManager::sceneCount() const {
        return mScene.size();
    }

    Scene* SceneManager::getActiveScene() const {
        return mIsActive;
    }

    Scene* SceneManager::createScene(const std::string &_str) {
        const auto ret = new Scene(_str);
        mScene.insert(ret);
        return ret;
    }

    void SceneManager::loadScene(size_t _sceneBuildIndex) {
        auto iter = mScene.begin();
        while ((*iter)->buildIndex() != _sceneBuildIndex) {
            ++iter;
        }
        (*iter)->getLoaded();
    }

    void SceneManager::loadScene(const std::string& _scenename) {
        auto iter = mScene.begin();
        while ((*iter)->getName() != _scenename) {
            ++iter;
        }
        (*iter)->getLoaded();
    }

    bool SceneManager::makeSceneActive(Scene *s) {
        return (mIsActive = s);
    }
}
#pragma once
#ifndef MX_SCENE_MANAGER_H_
#define MX_SCENE_MANAGER_H_

#include"MxScene.h"
#include<set>

namespace Mix {
    class SceneManager {
    public:
        size_t sceneCount() const;

        Scene* createScene(const std::string &_str);

        Scene* getActiveScene() const;

        bool makeSceneActive(Scene *s);

        void loadScene(size_t _sceneBuildIndex);

        void loadScene(const std::string& _scenename);

    private:
        std::set<Scene*> mScene;
        Scene *mIsActive;
    };
}

#endif

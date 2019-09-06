#pragma once

#ifndef MX_SCENE_MANAGER_HPP_
#define MX_SCENE_MANAGER_HPP_

#include "MxScene.h"
#include "../Engine/MxModuleBase.h"
#include "../../Scenes/MainScene.hpp"
#include <vector>

namespace Mix {
    class SceneManager final : public ModuleBase {
        friend class MixEngine;

    public:
        SceneManager() : mActiveScene(new Scene(ScenesDebug::mainSceneName, ScenesDebug::mainSceneOnLoad, 0)) {
            mScenes.push_back(mActiveScene);
        }

        ~SceneManager() {
            for(auto s : mScenes) {
                s->unload();
                delete s;
            }
        }

        Scene* createScene(std::string _name, HierarchyLoader _onLoad) {
            auto s = new Scene(std::move(_name), std::move(_onLoad), mScenes.size());
            mScenes.push_back(s);
            return s;
        }

        /** @note This function is extremely slow. */
        Scene* loadScene(const std::string& _name) const {
            auto iter = std::find_if(mScenes.cbegin(), mScenes.cend(), [&name = _name](auto* _s) { return _s->name() == name; });
            if(iter == mScenes.cend())
                return nullptr;
            if(mActiveScene) mActiveScene->unload();
            (*iter)->load();
            return *iter;
        }

        Scene* loadScene(const size_t _buildIdx) const {
            if(_buildIdx >= mScenes.size())
                return nullptr;
            if(mActiveScene) mActiveScene->unload();
            auto s = mScenes[_buildIdx];
            s->load();
            return s;
        }

        Scene* activeScene() const noexcept { return mActiveScene; }

    private:
        Scene* mActiveScene; /*std::vector<Scene*> mLoadedScenes;*/
        std::vector<Scene*> mScenes;

        void awake() override { mActiveScene->awake(); }
        void init() override { mActiveScene->init(); }
        void update() { mActiveScene->update(); }
        void fixedUpate() { mActiveScene->fixedUpate(); }
        void lateUpate() { mActiveScene->lateUpate(); }
    };
}

#endif

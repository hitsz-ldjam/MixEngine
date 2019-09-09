#pragma once

#ifndef MX_SCENE_MANAGER_HPP_
#define MX_SCENE_MANAGER_HPP_

#include "MxScene.h"
#include "../../MixEngine.h"
#include <vector>

namespace Mix {
    class SceneManager final : public ModuleBase {
        friend class MixEngine;

    public:
        SceneManager();

        ~SceneManager();

        Scene* createScene(std::string _name, HierarchyLoader _onLoad);

        /** @note This function is extremely slow. */
        Scene* loadScene(const std::string& _name) const;

        Scene* loadScene(const size_t _buildIdx) const;

        Scene* activeScene() const noexcept { return mActiveScene; }

        static SceneManager* Get() { return MixEngine::Instance().getModule<SceneManager>(); }

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

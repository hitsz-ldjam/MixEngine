#pragma once

#ifndef MX_SCENE_MANAGER_HPP_
#define MX_SCENE_MANAGER_HPP_

#include "MxScene.h"
#include "../Engine/MxModuleBase.h"

namespace Mix {
    //class SceneManager final : public ModuleBase {
    //    friend class MixEngine;

    //public:
    //    SceneManager();

    //    ~SceneManager();

    //    Scene* createScene(std::string _name, HierarchyLoader _onLoad);

    //    /** @note This function is extremely slow. */
    //    Scene* loadScene(const std::string& _name) const;

    //    Scene* loadScene(const size_t _buildIdx) const;

    //    Scene* activeScene() const noexcept { return mActiveScene; }

    //private:
    //    Scene* mActiveScene; /*std::vector<Scene*> mLoadedScenes;*/
    //    std::vector<Scene*> mScenes;

    //    void load() override { mActiveScene->awake(); }
    //    void init() override { mActiveScene->init(); }
    //    void update() { mActiveScene->update(); }
    //    void fixedUpate() { mActiveScene->fixedUpate(); }
    //    void lateUpate() { mActiveScene->lateUpate(); }
    //};


    class SceneManager final :public ModuleBase {
    public:
        static SceneManager* Get();

        SceneManager() = default;

        ~SceneManager() = default;

        std::shared_ptr<Scene> createScene(const std::string& _name);

        std::shared_ptr<Scene> getScene(const std::string& _name);

        std::shared_ptr<Scene> getScene(uint32_t _index);

        void loadScene(const std::string& _name);

        void loadScene(uint32_t _index);

        void setActiveScene(const std::shared_ptr<Scene>& _scene);

        void unloadScene(const std::string& _name);

        void unloadScene(uint32_t _index);

        uint32_t getSceneCount() const;

        const std::shared_ptr<Scene>& getActiveScene() const { return mActiveScene; }

        void load() override;

        void init() override;

        void sceneAwake();

        void sceneInit();

        void sceneUpdate();

        void sceneFixedUpdate();

        void sceneLateUpdate();

    private:
        friend GameObject;

        void registerGameObjectToMS(const HGameObject& _object);

        std::shared_ptr<Scene> mActiveScene;

        uint32_t mNextId = 0;
        std::unordered_map<std::string, uint32_t> mSceneNameIndexMap;
        std::unordered_map<uint32_t, std::shared_ptr<Scene>> mIndexSceneMap;
    };
}

#endif

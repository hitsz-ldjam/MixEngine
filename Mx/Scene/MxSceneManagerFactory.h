#pragma once

#ifndef MX_SCENE_MANAGER_H_
#define MX_SCENE_MANAGER_H_

#include <string>
#include <set>

namespace Mix {
    class Scene;

    // todo: implement this class
    class SceneManagerFactory final {
    public:
        SceneManagerFactory(const SceneManagerFactory&) = delete;
        ~SceneManagerFactory();

        //size_t sceneCount() const { return mScenes.size(); }

        //Scene* createScene(const std::string& _name);

        //const Scene* operator[](const size_t _buildIdx) const;

        //const Scene& operator[](const std::string& _name) const;

        //Scene* getActiveScene() const;

        //bool setActiveScene(Scene* _scene);

        //void loadScene(const size_t _buildIdx);

        //void loadScene(const std::string& _name);

    private:
        SceneManagerFactory() : mActiveScene(nullptr) {}

        Scene* mActiveScene; // Unused
        std::set<Scene*> mScenes;
    };
}

#endif

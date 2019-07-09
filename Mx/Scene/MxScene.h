#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include <string>
#include <set>

namespace Mix {
    class GameObject;

    class Scene final {
        friend class SceneManagerFactory;

    public:
        // todo: create through SceneMgr
        explicit Scene(std::string _name, const size_t _buildIdx = 0);

        // todo: implement copy ctor
        Scene(const Scene& _other) = delete;

        ~Scene() = default;

        void awake();
        void init();
        void update();
        void fixedUpate();
        void lateUpate();

        void addGameObject(GameObject* _obj) { if(_obj) mHierarchy.insert(_obj); }
        void removeGameObject(GameObject* _obj) { mHierarchy.erase(_obj); }

        std::string& name() noexcept { return mName; }
        const std::string& name() const noexcept { return mName; }
        size_t buildIndex() const noexcept { return mBuildIndex; }
        bool isLoaded() const noexcept { return mIsLoaded; }

        auto begin() noexcept { return mHierarchy.begin(); }
        auto begin() const noexcept { return mHierarchy.begin(); }
        auto end() noexcept { return mHierarchy.end(); }
        auto end() const noexcept { return mHierarchy.end(); }

        bool operator==(const Scene& _right) { return mHierarchy == _right.mHierarchy; }
        bool operator!=(const Scene& _right) { return !operator==(_right); }

    private:
        std::string mName;
        size_t mBuildIndex;
        bool mIsLoaded;
        std::set<GameObject*> mHierarchy;

        // todo: further set ups
        void load() noexcept { mIsLoaded = true; }
        void unload() noexcept { mIsLoaded = false; }
    };
}

#endif

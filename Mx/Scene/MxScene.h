#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include <set>
#include <string>
#include <functional>
#include <queue>

namespace Mix {
    class GameObject;
    class Camera;

    using Hierarchy = std::set<GameObject*>;

    /** @brief The function should return only the root GameObjects. */
    using HierarchyLoader = std::function<Hierarchy()>;

    class Scene final {
        friend class SceneManager;

    public:
        static const std::string defaultMainSceneName;
        static const HierarchyLoader defaultMainSceneOnLoad;

        Scene(const Scene&) = delete;
        ~Scene() { unload(); }
        const std::string& name() const noexcept { return mName; }
        size_t buildIdx() const noexcept { return mBuildIdx; }
        bool isLoaded() const noexcept { return mIsLoaded; }

        // bool isDirty() const noexcept { return !(mInstantiateObjs.empty() && mDestroyObjs.empty()); }

        auto getRootGameObjects() const noexcept { return Hierarchy(mRootObjs); }

        /** @note Using BFS. */
        template<typename _Pr>
        GameObject* findGameObjectIf(_Pr _pred) const;

        /** @note Calls Scene::findGameObjectIf() */
        GameObject* findGameObject(const std::string& _name) const;

        /** @note This function is slow. Consider storing the Camera. */
        Camera* mainCamera() const;

    private:
        std::string mName;
        HierarchyLoader mOnLoad;
        size_t mBuildIdx;

        bool mIsLoaded;
        Hierarchy mRootObjs;

        // todo: GameObject::Instantiate() & GameObject::Destroy()
        // Hierarchy mInstantiateObjs, mDestroyObjs;

        explicit Scene(std::string _name,
                       HierarchyLoader _onLoad,
                       const size_t _buildIdx) : mName(std::move(_name)),
                                                 mOnLoad(std::move(_onLoad)),
                                                 mBuildIdx(_buildIdx),
                                                 mIsLoaded(false) {}

        void load();
        void unload();

        void awake();
        void init();
        void update();
        void fixedUpate();
        void lateUpate();
    };

    template<typename _Pr>
    GameObject* Scene::findGameObjectIf(_Pr _pred) const {
        static_assert(std::is_convertible_v<std::invoke_result_t<_Pr, GameObject*>, bool>, "Invalid typename _Pr");
        std::queue<GameObject*> queue;
        for(auto obj : mRootObjs)
            queue.push(obj);
        while(!queue.empty()) {
            auto obj = queue.front();
            queue.pop();
            if(_pred(obj))
                return obj;
            auto children = obj->getAllChildren();
            for(auto child : children)
                queue.push(child);
        }
        return nullptr;
    }
}

#endif

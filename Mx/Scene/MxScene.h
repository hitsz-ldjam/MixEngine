#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include <set>
#include <string>
#include <functional>

#include "../GameObject/MxGameObject.h"
#include "../Graphics/MxRenderSceneInfo.h"

namespace Mix {
    //class GameObject;

    //using Hierarchy = std::set<GameObject*>;

    ///** @brief The function should return only the root GameObjects. */
    //using HierarchyLoader = std::function<Hierarchy()>;

    //class Scene final {
    //    friend class SceneManager;

    //public:
    //    static const std::string defaultMainSceneName;
    //    static const HierarchyLoader defaultMainSceneOnLoad;

    //    Scene(const Scene&) = delete;
    //    ~Scene() { unload(); }
    //    const std::string& name() const noexcept { return mName; }
    //    size_t buildIdx() const noexcept { return mBuildIdx; }
    //    bool isLoaded() const noexcept { return mIsLoaded; }
    //    // bool isDirty() const noexcept { return !(mInstantiateObjs.empty() && mDestroyObjs.empty()); }

    //    auto getRootGameObjects() const noexcept { return Hierarchy(mRootObjs); }

    //private:
    //    std::string mName;
    //    HierarchyLoader mOnLoad;
    //    size_t mBuildIdx;

    //    bool mIsLoaded;
    //    Hierarchy mRootObjs;
    //    // todo: GameObject::Instantiate() & GameObject::Destroy()
    //    // Hierarchy mInstantiateObjs, mDestroyObjs;

    //    explicit Scene(std::string _name,
    //                   HierarchyLoader _onLoad,
    //                   const size_t _buildIdx) : mName(std::move(_name)),
    //                                             mOnLoad(std::move(_onLoad)),
    //                                             mBuildIdx(_buildIdx),
    //                                             mIsLoaded(false) {}

    //    void load();
    //    void unload();

    //    void awake();
    //    void init();
    //    void update();
    //    void fixedUpate();
    //    void lateUpate();
    //};
    class SceneFiller;

    class Scene {
        friend class SceneManager;
        friend class GameObject;
        friend class SceneFiller;
    public:
        const std::string& getName() const { return mName; }

        ~Scene();

        /**
         * \brief Find and return the first handle of GameObject in scene with specified name
         * \param _name The name of the GameObject to find
         * \param _wholeScene If true seach all GameObjects in scene or just search root GameObjects
         * \return The first found GameObject with specified name, or null handle if no one found
         */
        HGameObject find(const std::string& _name, bool _wholeScene = false);

        void setFiller(const std::shared_ptr<SceneFiller>& _filler);

        void registerCamera(const HCamera& _camera);

        void unregisterCamera(const HCamera& _camera);

        void setMainCamera(const HCamera& _camera);

        HCamera getMainCamera();

        bool isActive() const { return mIsActive; }

        bool isLoaded() const { return mIsLoaded; }

        std::vector<HGameObject> getRootGameObjects() const;

        SceneRenderInfo _getRendererInfoPerFrame();

        uint32_t getIndex() const { return mIndex; }

        /** @note Using BFS. */
        template<typename _Pr>
        GameObject* findGameObjectIf(_Pr _pred) const;

        /** @note Calls Scene::findGameObjectIf() */
        GameObject* findGameObject(const std::string& _name) const;

        /** @note This function is slow. Consider storing the Camera. */
        Camera* mainCamera() const;

    private:
        Scene(const std::string& _name, uint32_t _index);

        static std::shared_ptr<Scene> Create(const std::string& _name, uint32_t _index);

        /** \brief Call awake() of Objects in whole scene. */
        void sceneAwake();

        /** \brief Call init() of Objects in whole scene. */
        void sceneInit();

        /** \brief Call update() of Objects in whole scene. */
        void sceneUpdate();

        /** \brief Call fixedUpdate() of Objects in whole scene. */
        void sceneFixedUpate();

        /** \brief Call lateUpdate() of Objects in whole scene. */
        void sceneLateUpate();

        void scenePostRender();
        
        /** \brief Nake this scene active . */
        void activate() { mIsActive = true; }

        /** \brief Nake this scene inactive . */
        void inactivate() { mIsActive = false; }

        /** \brief Register a new GameObject in the scene. \n
         *         The GameObjects registered during main loop will be awaked and initialized at the end of the frame. \n
         *         The GameObjects registered before scene awaked will be awaked in sceneAwake().
         */
        void registerGameObject(const HGameObject& _object);

        /** \brief Unregister a GameObject from the scene. */
        void unregisterGameObject(const HGameObject& _object);

        /** \brief Flush all GameObjects registered in the scene but not yet added to the scene */
        void flushNewAddedObject();

        /** \brief Set the index of the scene */
        void setIndex(uint32_t _index) { mIndex = _index; }

        /** \brief Load the scene. This will call SceneFiller if has. \n
         *         All objects in the scene will be created and all resource will be loaded.  */
        void load();

        /** \brief Unload the scene. All objects will be destroyed. */
        void unload();

        void findRendererRecur(SceneRenderInfo& _info, const HGameObject& _object);

    private:
        std::weak_ptr<Scene> mThisPtr;
        std::string mName;
        std::unordered_map<uint64_t, HGameObject> mRootObjects;
        std::vector<HGameObject> mNewRootObjects;
        std::vector<HGameObject> mNeedAwakeAndInit;

        std::vector<HCamera> mRegisteredCamera;
        HCamera mMainCamera;

        uint32_t mIndex = 0;
        bool mIsActive = false;

        bool mIsLoaded = false;
        std::shared_ptr<SceneFiller> mFiller;

    };

    class SceneFiller {
        friend class Scene;
    public:
        SceneFiller() = default;

    protected:
        HGameObject createGameObject(const std::string& _name, Tag _tag = "", const LayerIndex _layerIndex = 0, Flags<GameObjectFlags> _flags = {}) const;

        virtual void fillScene(const std::shared_ptr<Scene>& _scene) = 0;

    private:
        void fill(const std::shared_ptr<Scene>& _scene);

        std::shared_ptr<Scene> mTemp;
    };

    class DefaultSceneFiller :public SceneFiller {
    public:
        using SceneFiller::SceneFiller;

        ~DefaultSceneFiller() = default;

    private:
        void fillScene(const std::shared_ptr<Scene>& _scene) override;
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

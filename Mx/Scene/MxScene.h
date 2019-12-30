#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include <queue>
#include <string>
#include <functional>

#include "../GameObject/MxGameObject.h"
#include "../Graphics/MxRenderInfo.h"
#include <unordered_set>

namespace Mix {
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
        HGameObject findGameObject(const std::string& _name, bool _wholeScene = false);

        /** \brief Set a filler for this scene. */
        void setFiller(const std::shared_ptr<SceneFiller>& _filler);

        /** \brief Register a camera to the scene. The camera that is registered can be set as main camera. */
        void registerCamera(const HCamera& _camera);

        /** \brief Unregister a camera to the scene. */
        void unregisterCamera(const HCamera& _camera);

        /**
         *\brief Set the main camera of the scene. The main camera will be used during rendering.
         *\note  The caerma should be registered to the scene before being set as main camera.
         */
        void setMainCamera(const HCamera& _camera);

        /** \brief Get the main camera of the scene. */
        HCamera getMainCamera();

        /** \brief Check if the scene is currently active. */
        bool isActive() const { return mIsActive; }

        /** \brief Check if the scene has been loaded. */
        bool isLoaded() const { return mIsLoaded; }

        /** \brief Get all root GameObjects in the scene. */
        std::vector<HGameObject> getRootGameObjects() const;

        FrameSceneInfo _getFrameSceneInfo();

        uint32_t getIndex() const { return mIndex; }

        /**
         *\brief Return the first found gameobject that satisfies the specified condition.
         *\param _pred A callable object that defines the condition to be satisfied by the element.
         *\note  This may be expensive when the amount of gameobjects in the scene is too big.\n
         *       Avoid calling this as possible as you can.
         */
        template<typename _Pr>
        HGameObject findGameObjectIf(_Pr _pred) const;

    private:
        Scene(const std::string& _name, uint32_t _index);

        static std::shared_ptr<Scene> Create(const std::string& _name, uint32_t _index);

        /** \brief Call awake() of Objects in whole scene. */
        // void sceneAwake();

        /** \brief Call init() of Objects in whole scene. */
        // void sceneInit();

        /** \brief Call update() of Objects in whole scene. */
        void sceneUpdate();

        /** \brief Call fixedUpdate() of Objects in whole scene. */
        void sceneFixedUpate();

        /** \brief Call lateUpdate() of Objects in whole scene. */
        void sceneLateUpate();

        /** \brief Called in postRender(). */
        void scenePostRender();

        /** \brief Nake this scene active . */
        void activate();

        /** \brief Nake this scene inactive . */
        void inactivate() { mIsActive = false; }

        /** \brief Register a new GameObject in the scene. \n
         *         The GameObjects registered during main loop will be awaked and initialized at the end of the frame. \n
         *         The GameObjects registered before scene awaked will be awaked in sceneAwake().
         */
        void registerGameObject(const HGameObject& _object);

        /** \brief Unregister a GameObject from the scene. */
        void unregisterGameObject(const HGameObject& _object);

        void registerBehaviour(const HBehaviour& _behaviour);

        void unregisterBehaviour(const HBehaviour& _behaviour);

        void rootGameObjectChanged(const HGameObject& _object);

        /** \brief Flush all GameObjects registered in the scene but not yet added to the scene */
        void flushNewAddedBehaviour();

        /** \brief Set the index of the scene */
        void setIndex(uint32_t _index) { mIndex = _index; }

        /** \brief Load the scene. This will call SceneFiller if has. \n
         *         All objects in the scene will be created and all resource will be loaded.  */
        void load();

        /** \brief Unload the scene. All objects will be destroyed. */
        void unload();

        void registerRenderer(const HRenderer& _renderer);

    private:
        static void FillFrameSceneInfo(FrameSceneInfo& _info, const HGameObject& _object);


        std::weak_ptr<Scene> mThisPtr;
        std::string mName;
        std::unordered_map<uint64_t, HGameObject> mRootObjects;

        std::map<uint64_t, HBehaviour> mBehaviours;
        std::vector<HBehaviour> mNewBehaviours;

        std::vector<HRenderer> mRenderers;
        // std::unordered_set<HRenderer> mRemovedRenderers;

        std::vector<HCamera> mRegisteredCamera;
        HCamera mMainCamera;

        uint32_t mIndex = 0;
        bool mIsActive = false;

        bool mIsLoaded = false;
        std::shared_ptr<SceneFiller> mFiller;

    };


    template<typename _Pr>
    HGameObject Scene::findGameObjectIf(_Pr _pred) const {
        static_assert(std::is_invocable_r_v<bool, _Pr, HGameObject>,
                      "The callable object should accept a HGameObject as paramter and the return vaule should be convitible to bool");
        std::queue<HGameObject> queue;
        for (auto obj : mRootObjects) {
            queue.push(obj.second);
        }

        while (!queue.empty()) {
            auto obj = queue.front();
            queue.pop();

            if (_pred(obj)) {
                return obj;
            }

            auto children = obj->getAllChildren();
            for (auto child : children) {
                queue.push(child);
            }
        }

        return nullptr;
    }


    /**
     * \brief An abstract base for the class for a filler of scene. The filler will be used to filler the scene with SceneObjects at the time the scene is loaded.
     */
    class SceneFiller {
        friend class Scene;
    public:
        virtual ~SceneFiller() = default;

        SceneFiller() = default;

    protected:
        HGameObject createGameObject(const std::string& _name, const Tag& _tag = "", const LayerIndex _layerIndex = 0, Flags<GameObjectFlags> _flags = {}) const;

        virtual void fillScene(const std::shared_ptr<Scene>& _scene) = 0;

    private:
        void fill(const std::shared_ptr<Scene>& _scene);

        std::shared_ptr<Scene> mTemp;
    };

    /**
     * \brief Default scene filler for scene with no specified scene filler. This filler will add an GameObject called "MainCamera" to the scene. \n
     *        The GameObject has a Component camera with it.
     */
    class DefaultSceneFiller :public SceneFiller {
    public:
        using SceneFiller::SceneFiller;

        ~DefaultSceneFiller() = default;

    private:
        void fillScene(const std::shared_ptr<Scene>& _scene) override;
    };

}

#endif

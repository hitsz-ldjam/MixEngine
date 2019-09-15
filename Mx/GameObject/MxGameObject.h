#pragma once

#ifndef MX_GAME_OBJECT_H_
#define MX_GAME_OBJECT_H_

#include "../Component/Behaviour/MxBehaviour.h"
#include "../Component/Transform/MxTransform.h"
#include "../Scene/MxSceneObject.h"
#include "../Scene/MxSceneObjectManager.h"
#include <set>
#include "../Scene/MxSceneObjectHandle.h"

namespace Mix {
    class Scene;
    class SceneManager;
    class SceneFiller;
    class Model;

    using LayerIndex = uint32_t;
    using Tag = std::string;

    enum class GameObjectFlags {
        IsStatic = 0x02
    };

    MX_ALLOW_FLAGS_FOR_ENUM(GameObjectFlags);

    class GameObject final : public SceneObject {
        MX_DECLARE_RTTI;

        friend Scene;
        friend SceneManager;
        friend SceneFiller;
        friend Model;
    public:
        ~GameObject();

        static HGameObject Create(const std::string& _name, Tag _tag = "", LayerIndex _layerIndex = 0, Flags<GameObjectFlags> _flags = {});

        void destroy(bool _immediate = false);

        HGameObject getHandle() const { return mThisHandle; }

        GameObject(const GameObject& _obj) = delete;

        GameObject& operator=(const GameObject& _obj) = delete;


        Flags<GameObjectFlags> getFlags() const { return mFlags; }

        void _setFlags(Flags<GameObjectFlags> _flags);

        void _unsetFlags(Flags<GameObjectFlags> _flags);

    private:
        explicit GameObject(const std::string& _name, const Tag& _tag = "", LayerIndex _layerIndex = 0, Flags<GameObjectFlags> _flags = {});

        static HGameObject CreateInternal(const std::shared_ptr<GameObject>& _gameObject);

        static HGameObject CreateInternal(const std::string& _name, const Tag& _tag = "", LayerIndex _layerIndex = 0, Flags<GameObjectFlags> _flags = {}, const std::shared_ptr<Scene>& _scene = nullptr);

        void destroyInternal(SceneObjectHandleBase& _handle, bool _immediate = false) override;

        HGameObject mThisHandle;
        Flags<GameObjectFlags> mFlags;

    public:

        //////////////////////////////////////////////////////////////////
        //                          Components                          //
        //////////////////////////////////////////////////////////////////

        // DO NOT use this function yet.
        GameObject& operator=(GameObject&& _obj) noexcept;

        /**
         *  @brief Construct a Component of type _Ty and add it to this GameObject.
         *  @return A handle to the Component
         */
        template<typename _Ty, typename... _Args>
        SceneObjectHandle<_Ty> addComponent(_Args&&... _args);

        /**
         *  @brief Get the pointer to the Component of type _Ty attached to this GameObject.
         *  @return A handle to the Component, nullptr if not found
         */
        template<typename _Ty>
        SceneObjectHandle<_Ty> getComponent();

        /**
         *  @brief Get all Components of type _Ty attached to this GameObject.
         *  @return A vector containing handles of Components found
         */
        template<typename _Ty>
        std::vector<SceneObjectHandle<_Ty>> getComponents();

        /**
         *  @brief Check if a components of Specified type is attached to this GameObject.
         *  @return True if the object has a component of specified type.
         */
        template<typename _Ty>
        bool hasComponent();

        /**
         * Removes the component from this object, and deallocates it.
         *
         * @param	_component	The component to destroy.
         * @param	_immediate	If true, the component will be deallocated and become unusable right away. Otherwise
         *							the deallocation will be delayed to the end of frame (preferred method).
         */
        void removeComponent(const HComponent& _component, bool _immediate = false);


        std::vector<HComponent>& _getComponents() { return mComponents; }

    private:
        void addAndInitializeComponent(const HComponent& _component);

        HComponent getComponent(const Rtti& _type) const;

        std::vector<HComponent> mComponents;
        std::vector<HBehaviour> mBehaviours;

        //////////////////////////////////////////////////////////////////
        //                          Hierarchy                           //
        //////////////////////////////////////////////////////////////////

    public:
        /*
         * \brief Set the parent of this GameObject and remove the object from its parent.
         */
        void setParent(const HGameObject& _parent, bool _keepWorldTransform = true);

        /** \return A handle to the parent GameObject or null is this GameObject is a root node*/
        HGameObject getParent() const noexcept { return mParent; }

        /**
         * \brief Get a child of this GameObject
         * \param _index The index of the child in children array, starts from zero
         * \return A child GameObject
         */
        HGameObject getChild(uint32_t _index) const;

        /**
         * \brief Get the index of the specified child.
         * \param _child The child GameObject
         * \return The index of the child or -1 if not found
         */
        int indexOfChild(const HGameObject& _child) const;

        /**
         * \brief Get the amount of child GameObjects
         */
        uint32_t getChildrenCount() const { return static_cast<uint32_t>(mChildren.size()); }

        /**
         * \brief Get the scene this object belongs to.
         */
        std::shared_ptr<Scene> getScene() const;

        /**
         * \brief Find the child object with the specified name
         * \param _name The name of the child to find
         * \param _recursive Search the whole hierarchy using DFS
         * \return First found GameObject or empty if none foune
         */
        HGameObject findChild(const std::string& _name, bool _recursive = true);

        /**
         * \brief Find all child objects with the specified name
         * \param _name The name of the children to find
         * \param _recursive Search the whole hierarchy using DFS
         * \return All found GameObject or empty if none foune
         */
        std::vector<HGameObject> findChildren(const std::string& _name, bool _recursive = true);

        /** @brief Activates/Deactivates the GameObject, depending on the given true or false value. */
        void setActive(bool _active);

        auto getAllChildren() const noexcept {
            return std::vector<HGameObject>(mChildren.begin(), mChildren.end());
        }

        // HSceneObject clone(bool _instantiate = true);

        Transform& transform() noexcept { return *mTransform; }

        const Transform& transform() const noexcept { return *mTransform; }

        HTransform transformHandle() const noexcept { return static_scene_object_cast<Transform>(mTransform->getHandle()); }

        /** \brief Get the Tag of this GameObject. */
        const auto& getTag() const noexcept { return mTag; }

        /** \brief Get the Layer this GameObject belongs to. */
        const auto& getLayer() const noexcept { return mLayer; }

        /** \note Static-ness should be defined in ctor. */
        bool isStatic() const noexcept { return mFlags.isSet(GameObjectFlags::IsStatic); }

        /** \brief Check if this GameObject itself is active. */
        bool activeSelf() const noexcept { return mActiveSelf; }

        /** \brief Check if this GameObject is active in hierarchy. */
        bool activeInHierarchy() const noexcept { return mActiveInHierarchy; }

    private:
         /** \brief Add a GameObject to this GameObject */
        void addChild(const HGameObject& _object, bool _keepWorldTransform = true);

        /** \brief Remove a child from this GameObject. */
        void removeChild(const HGameObject& _object);

        void setScene(const std::shared_ptr<Scene>& _scene);

        std::shared_ptr<Scene> mScene;
        HGameObject mParent;
        std::vector<HGameObject> mChildren;
        bool mActiveSelf = true;
        bool mActiveInHierarchy = true;

        HTransform mTransform;

        Tag mTag;
        LayerIndex mLayer;

        /** @brief Insert _ptr into a shortlisted set of Behaviour if it points to what derived from Behaviour. */
        template<typename _Ty>
        std::enable_if_t<std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty* _ptr) { mBehaviours.insert(_ptr); }

        /** @brief This function is called when _Ty is not derived from Behaviour. */
        template<typename _Ty>
        std::enable_if_t<!std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty*) {}

        void awake() {
            for (auto& behaviour : mBehaviours)
                behaviour->awake();
            for (auto& c : mChildren)
                c->awake();
        }

        void init() {
            for (auto& behaviour : mBehaviours)
                behaviour->init();
            for (auto& c : mChildren)
                c->init();
        }

        void update() {
            for (auto& behaviour : mBehaviours)
                behaviour->update();
            for (auto& c : mChildren)
                c->update();
        }

        void fixedUpdate() {
            for (auto& behaviour : mBehaviours)
                behaviour->fixedUpdate();
            for (auto& c : mChildren)
                c->fixedUpdate();
        }

        void lateUpdate() {
            for (auto& behaviour : mBehaviours)
                behaviour->lateUpdate();
            for (auto& c : mChildren)
                c->lateUpdate();
        }

        // todo: further setups

        // void addToScene(Scene* _scene);

        // void removeFromScene(Scene* _scene);

        // ----- static variables and functions -----

    //public:
    //    /**
    //     *  @brief Find a GameObject named _name
    //     *  @return A pointer to the GameObject, nullptr if not found
    //     */
    //    static GameObject* Find(const std::string& _name);

    //    /**
    //     *  @brief Find all GameObjects with Tag _tag
    //     *  @return A vector containing pointers to those GameObjects
    //     */
    //    static std::vector<GameObject*> FindGameObjectsWithTag(const Tag& _tag);

    //    /**
    //     *  @brief Find a GameObject with Tag _tag
    //     *  @return A pointer to the first GameObject been found, nullptr if not found
    //     */
    //    static GameObject* FindGameObjectWithTag(const Tag& _tag);

    //private:
    //    static std::vector<GameObject*> sGameObjList;

    //    /** @brief Add GameObject _obj points to to global GameObject list */
    //    static void AddGameObject(GameObject* _obj) {
    //        if (_obj)
    //            sGameObjList.push_back(_obj);
    //    }

    //    /** @brief Remove the GameObject _obj points to fom global GameObject list */
    //    static void RemoveGameObject(GameObject* _obj) {
    //        auto it = std::find(sGameObjList.cbegin(), sGameObjList.cend(), _obj);
    //        if (it != sGameObjList.cend())
    //            sGameObjList.erase(it);
    //    }

    };

    // ----- template function implementations -----

    //template<typename _Ty>
    //inline _Ty* GameObject::addComponent() {
    //    // if type _Ty isn't derived from Component
    //    static_assert(std::is_base_of_v<Component, _Ty>, "A component must be derived from class Component");

    //    _Ty* t = new _Ty();
    //    t->setGameObject(this);
    //    mComponents.insert(t);

    //    addBehaviour(t);

    //    return t;
    //}

    template<typename _Ty, typename... _Args>
    SceneObjectHandle<_Ty> GameObject::addComponent(_Args&&... _args) {
        // if type _Ty isn't derived from Component
        static_assert(std::is_base_of_v<Component, _Ty>, "A component must be derived from class Component");

        std::shared_ptr<_Ty> component = std::shared_ptr<_Ty>(new _Ty(std::forward<_Args>(_args)...));

        HComponent componentHandle = static_scene_object_cast<Component>(SceneObjectManager::Get()->registerObject(component));

        addAndInitializeComponent(componentHandle);
        return static_scene_object_cast<_Ty>(componentHandle);
    }

    template<typename _Ty>
    SceneObjectHandle<_Ty> GameObject::getComponent() {
        static_assert(std::is_base_of_v<Component, _Ty>, "Specified type is not a Component");

        return static_scene_object_cast<_Ty>(getComponent(_Ty::GetType()));
    }

    template<typename _Ty>
    std::vector<SceneObjectHandle<_Ty>> GameObject::getComponents() {
        static_assert(std::is_base_of_v<Component, _Ty>(), "Specified type is not a Component");

        std::vector<SceneObjectHandle<_Ty>> results;

        for (auto comp : mComponents) {
            if (comp->getType().isSameType(_Ty::GetType()) || comp->getType().isDerivedFrom(_Ty::GetType()))
                results.push_back(comp);
        }

        return results;
    }

    template <typename _Ty>
    bool GameObject::hasComponent() {
        static_assert(std::is_base_of_v<Component, _Ty>(), "Specified type is not a Component");

        for (auto comp : mComponents) {
            if (comp->getType().isDerivedFrom(_Ty::GetType()))
                return true;
        }

        return false;
    }

    /*template<typename _Ty>
    _Ty* GameObject::getComponentInChildren() {
        _Ty* ptr = getComponent<_Ty>();

        if (!ptr) {
            for (auto child : mChildren) {
                if ((ptr = child->getComponentInChildren<_Ty>()))
                    return ptr;
            }
        }

        return nullptr;
    }*/
}

#endif

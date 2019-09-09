#pragma once

#ifndef MX_GAME_OBJECT_H_
#define MX_GAME_OBJECT_H_

#include "../Component/Behaviour/MxBehaviour.h"
#include "../Component/Transform/MxTransform.h"
#include <set>

namespace Mix {
    class Scene;

    using LayerIndex = uint32_t;
    using Tag = std::string;

    // deprecated
    struct GameObjectConInfo {
        std::string name;
        Tag tag;
        LayerIndex layerIndex;
        const bool isStatic;

        explicit GameObjectConInfo(std::string _name,
                                   Tag _tag = "",
                                   const LayerIndex _layerIndex = 0,
                                   const bool _isStatic = false) : name(std::move(_name)),
                                                                   tag(std::move(_tag)),
                                                                   layerIndex(_layerIndex),
                                                                   isStatic(_isStatic) {}
    };

    class GameObject final : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

        friend Scene;

    public:
        /** @note Default ctor for RTTI. DO NOT use this ctor. */
        GameObject();

        explicit GameObject(std::string _name,
                            Tag _tag = "",
                            const LayerIndex _layerIndex = 0,
                            const bool _isStatic = false);

        // deprecated
        GameObject(const GameObjectConInfo& _info);

        GameObject(const GameObject& _obj) = delete;

        GameObject& operator=(const GameObject& _obj) = delete;

        // DO NOT use this function yet.
        GameObject(GameObject&& _obj) noexcept;

        // DO NOT use this function yet.
        GameObject& operator=(GameObject&& _obj) noexcept;

        ~GameObject();

        /**
         *  @brief Add Component _comp to this GameObject.
         *  @return _comp itself
         */
        Component* addComponent(Component* _comp);

        /**
         *  @brief Add a Component of type _Ty to this GameObject using default ctor.
         *  @return A pointer to the Component
         */
        template<typename _Ty>
        _Ty* addComponent();

        /**
         *  @brief Construct a Component of type _Ty and add it to this GameObject.
         *  @return A pointer to the Component
         */
        template<typename _Ty, typename... _Args>
        _Ty* addComponent(_Args&&... _args);

        /**
         *  @brief Get the pointer to the Component of type _Ty attached to this GameObject.
         *  @return A pointer to the Component, nullptr if not found
         */
        template<typename _Ty>
        _Ty* getComponent();

        /**
         *  @brief Get all Components of type _Ty attached to this GameObject.
         *  @return A vector containing pointers of Components found
         */
        template<typename _Ty>
        std::vector<_Ty*> getComponents();

        /** @brief Get Component of type _Ty in children of this GameObject using DFS */
        template<typename _Ty>
        _Ty* getComponentInChildren();

        /**
         *  @brief Remove _comp from this GameObject, does nothing unless attached to this GameObject.
         *  @note _comp is NOT deleted after function call.
         */
        void removeComponent(Component* _comp);

        /** @return A pointer to the parent GameObject */
        GameObject* parent() const noexcept { return mParent; }

        /** @brief Add _obj as a child of this GameObject and remove it from its original parent. */
        void addChild(GameObject* _obj);

        /** 
         *  @brief Remove _obj from this GameObject.
         *  @note _obj is NOT deleted after function call.
         */
        void removeChild(GameObject* _obj);

        auto getAllChildren() const noexcept {
            return std::vector<GameObject*>(mChildren.begin(), mChildren.end());
        }

        Transform& transform() noexcept { return *mTransform; }
        const Transform& transform() const noexcept { return *mTransform; }

        /** @brief Get the Tag of this GameObject. */
        const auto& getTag() const noexcept { return mTag; }

        /** @brief Get the Layer this GameObject belongs to. */
        const auto& getLayer() const noexcept { return mLayer; }

        /** @note Static-ness should be defined in ctor. */
        bool isStatic() const noexcept { return mIsStatic; }

        /** @brief Check if this GameObject itself is active. */
        bool activeSelf() const noexcept { return mActiveSelf; }

        /** @brief Check if this GameObject is active in hierarchy. */
        bool activeInHierarchy() const noexcept { return mActiveInHierarchy; }

        /** @brief Activates/Deactivates the GameObject, depending on the given true or false value. */
        void setActive(const bool _active);

        /** @brief Get the Scene this GameObject is part of. */
        const Scene* scene() const noexcept { return mScene; }

    private:
        GameObject* mParent;
        std::set<GameObject*> mChildren;

        std::set<Component*> mComponents;
        std::set<Behaviour*> mBehaviours;
        Transform* mTransform;

        Tag mTag;
        LayerIndex mLayer;
        bool mIsStatic, mActiveSelf, mActiveInHierarchy;

        Scene* mScene;

        /** @brief Insert _ptr into a shortlisted set of Behaviour if it points to what derived from Behaviour. */
        template<typename _Ty>
        std::enable_if_t<std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty* _ptr) { mBehaviours.insert(_ptr); }

        /** @brief This function is called when _Ty is not derived from Behaviour. */
        template<typename _Ty>
        std::enable_if_t<!std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty*) {}

        void awake() {
            for(auto behaviour : mBehaviours)
                behaviour->awake();
            for(auto c : mChildren)
                c->awake();
        }

        void init() {
            for(auto behaviour : mBehaviours)
                behaviour->init();
            for(auto c : mChildren)
                c->init();
        }

        void update() {
            for(auto behaviour : mBehaviours)
                behaviour->update();
            for(auto c : mChildren)
                c->update();
        }

        void fixedUpdate() {
            for(auto behaviour : mBehaviours)
                behaviour->fixedUpdate();
            for(auto c : mChildren)
                c->fixedUpdate();
        }

        void lateUpdate() {
            for(auto behaviour : mBehaviours)
                behaviour->lateUpdate();
            for(auto c : mChildren)
                c->lateUpdate();
        }

        // todo: further Scene setups
        void addToScene(Scene* _scene);
        void removeFromScene(Scene* _scene);

        // ----- static variables and functions -----

    public:
        /**
         *  @brief Find a GameObject named _name
         *  @return A pointer to the GameObject, nullptr if not found
         */
        static GameObject* Find(const std::string& _name);

        /**
         *  @brief Find all GameObjects with Tag _tag
         *  @return A vector containing pointers to those GameObjects
         */
        static std::vector<GameObject*> FindGameObjectsWithTag(const Tag& _tag);

        /**
         *  @brief Find a GameObject with Tag _tag
         *  @return A pointer to the first GameObject been found, nullptr if not found
         */
        static GameObject* FindGameObjectWithTag(const Tag& _tag);

    private:
        static std::vector<GameObject*> sGameObjList;

        /** @brief Add GameObject _obj points to to global GameObject list */
        static void AddGameObject(GameObject* _obj) {
            if(_obj)
                sGameObjList.push_back(_obj);
        }

        /** @brief Remove the GameObject _obj points to fom global GameObject list */
        static void RemoveGameObject(GameObject* _obj) {
            auto it = std::find(sGameObjList.cbegin(), sGameObjList.cend(), _obj);
            if(it != sGameObjList.cend())
                sGameObjList.erase(it);
        }
    };

    // ----- template function implementations -----

    template<typename _Ty>
    inline _Ty* GameObject::addComponent() {
        // if type _Ty isn't derived from Component
        static_assert(std::is_base_of_v<Component, _Ty>, "A component must be derived from class Component");

        _Ty* t = new _Ty();
        t->setGameObject(this);
        mComponents.insert(t);

        addBehaviour(t);

        return t;
    }

    template<typename _Ty, typename... _Args>
    inline _Ty* GameObject::addComponent(_Args&&... _args) {
        // if type _Ty isn't derived from Component
        static_assert(std::is_base_of_v<Component, _Ty>, "A component must be derived from class Component");

        _Ty* t = new _Ty(std::forward<_Args>(_args)...);
        t->setGameObject(this);
        mComponents.insert(t);

        addBehaviour(t);

        return t;
    }

    template<typename _Ty>
    inline _Ty* GameObject::getComponent() {
        _Ty* result;

        for(auto comp : mComponents) {
            if((result = dynamic_cast<_Ty*>(comp)))
                return result;
        }

        return nullptr;
    }

    template<typename _Ty>
    inline std::vector<_Ty*> GameObject::getComponents() {
        std::vector<_Ty*> results;
        _Ty* result;

        for(auto comp : mComponents) {
            if((result = dynamic_cast<_Ty*>(comp)))
                results.push_back(result);
        }

        return results;
    }

    template<typename _Ty>
    inline _Ty* GameObject::getComponentInChildren() {
        _Ty* ptr = getComponent<_Ty>();

        if(!ptr) {
            for(auto child : mChildren) {
                if((ptr = child->getComponentInChildren<_Ty>()))
                    return ptr;
            }
        }

        return nullptr;
    }
}

#endif

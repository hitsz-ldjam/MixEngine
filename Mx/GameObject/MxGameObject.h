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

    struct GameObjectCreateInfo {
        std::string name = "";
        Tag tag = "";
        LayerIndex layer = 0;
        bool isStatic = false;

        GameObjectCreateInfo(std::string _name = "",
                                   Tag _tag = "",
                                   const LayerIndex _layerIndex = 0,
                                   const bool _isStatic = false)
            : name(std::move(_name)),
              tag(std::move(_tag)),
              layer(_layerIndex),
              isStatic(_isStatic) {}
    };

    class GameObject : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

        friend Scene;

    public:
        /** @note Default ctor for RTTI. DO NOT use this ctor. */
        GameObject();

        GameObject(const GameObjectCreateInfo& _info);

        GameObject(const GameObject& _obj) = delete;

        // DO NOT use this function yet. There might be bugs.
        GameObject(GameObject&& _obj) noexcept;

        GameObject& operator=(const GameObject& _obj) = delete;

        // DO NOT use this function yet. There might be bugs.
        GameObject& operator=(GameObject&& _obj) noexcept;

        virtual ~GameObject();

        /**
         *  @brief Add Component _comp to this GameObject
         *  @return The pointer to the Component
         */
        Component* addComponent(Component* _comp);

        /**
         *  @brief Add a Component of type _Ty to this GameObject, using default constructor to create the Component
         *  @return The pointer to the Component
         */
        template<typename _Ty>
        _Ty* addComponent();

        /**
         *  @brief Construct a Component of type _Ty and add it to this GameObject
         *  @return The pointer to the Component
         */
        template<typename _Ty, typename... _Args>
        _Ty* addComponent(_Args&&... _args);

        /**
         *  @brief Get the pointer to the Component of type _Ty that attached to this GameObject
         *  @return The pointer to the Component, return nullptr if not found
         */
        template<typename _Ty>
        _Ty* getComponent();

        /**
         *  @brief Get all Components of type _Ty that attached to this GameObject
         *  @return A vector containing pointers of Components that been found
         */
        template<typename _Ty>
        std::vector<_Ty*> getComponents();

        /** @brief Get Component of type _Ty in children of this GameObject using DFS */
        template<typename _Ty>
        _Ty* getComponentInChildren();

        /**
         *  @brief Removes Component that _comp points to,
         *  does nothing if _comp isn't attached to this GameObject
         */
        void removeComponent(Component* _comp);

        /**
         *  @brief Add _obj as a child of this GameObject.\n
         *  Automatically remove it from its original parent.
         */
        void addChild(GameObject* _obj);

        /** @brief Remove a child that _obj points to from this GameObject */
        void removeChild(GameObject* _obj);

        auto getAllChildren() const {
            return std::vector<GameObject*>(mChildren.begin(), mChildren.end());
        }

        bool activeInHierarchy() const { return mActiveInHierarchy; }

        /** @brief Check if this GameObject is active */
        bool activeSelf() const { return mActiveSelf; }

        /** @brief Activates/Deactivates the GameObject, depending on the given true or false value. */
        void setActive(const bool _active);

        /** @note There is not getter of this field because altering this will affect its physics. */
        bool isStatic() const { return mIsStatic; }

        /** @brief Get the index of the layer that this GameObject belongs to */
        const auto& getLayer() const { return mLayer; }

        /** @brief Get the tag attached to this GameObject */
        const auto& getTag() const { return mTag; }

        Transform& transform() noexcept { return *mTransform; }
        const Transform& transform() const noexcept { return *mTransform; }

        GameObject* parent() const noexcept { return mParent; }

    protected:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;
        std::set<Behaviour*> mBehaviours;

        bool mActiveInHierarchy, mActiveSelf, mIsStatic;
        Tag mTag;
        LayerIndex mLayer;
        Transform* mTransform;
        Scene* mScene;

    private:
        void awake() {
            for(auto behaviour : mBehaviours)
                behaviour->awake();
        }

        void init() {
            for(auto behaviour : mBehaviours)
                behaviour->init();
        }

        void update() {
            for(auto behaviour : mBehaviours)
                behaviour->update();
        }

        void fixedUpdate() {
            for(auto behaviour : mBehaviours)
                behaviour->fixedUpdate();
        }

        void lateUpdate() {
            for(auto behaviour : mBehaviours)
                behaviour->lateUpdate();
        }

        /** @brief Insert _ptr into a shortlisted set of Mix::Behaviours
         *  if and only if _ptr points to an object derived from Mix::Behaviour.
         */
        template<typename _Ty>
        std::enable_if_t<std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty* _ptr) { mBehaviours.insert(_ptr); }

        /** @brief This function is called when _Ty is not derived from Mix::Behaviour. */
        template<typename _Ty>
        std::enable_if_t<!std::is_base_of_v<Behaviour, _Ty>> addBehaviour(_Ty*) {}

        // ----- static variables and functions -----

    public:
        /**
         *  @brief Get a GameObject named _name
         *  @return The Pointer to the GameObject, return nullptr if not found
         */
        static GameObject* Find(const std::string& _name);

        /**
         *  @brief Get all GameObjects with tag _tag
         *  @return A vector containing pointers to GameObjects
         */
        static std::vector<GameObject*> FindGameObjectsWithTag(const Tag& _tag);

        /**
         *  @brief Get a GameObject with tag _tag
         *  @return The pointer to the first GameObject been found, return nullptr if not found
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
        static_assert(std::is_base_of_v<Component, _Ty>, "A Component must be a derived class of Component");

        _Ty* t = new _Ty();
        t->setGameObject(this);
        mComponents.insert(t);

        addBehaviour(t);

        return t;
    }

    template<typename _Ty, typename... _Args>
    inline _Ty* GameObject::addComponent(_Args&&... _args) {
        // if type _Ty isn't derived from Component
        static_assert(std::is_base_of_v<Component, _Ty>, "A Component must be a derived class of Component");

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

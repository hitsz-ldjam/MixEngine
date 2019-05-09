#pragma once

#ifndef _MX_GAME_OBJECT_H_
#define _MX_GAME_OBJECT_H_

#include "MxException.hpp"
#include "MxObject.h"
#include "MxTransform.h"
#include <vector>
#include <set>
#include <type_traits>
#include "../Resource/MxResourceBase.h"


namespace Mix {
    class Component;
    class Transform;

    // todo: finish this class
    class GameObject : public Object {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        GameObject() :mParent(nullptr) {
            AddGameObj(this);
            AddComponent<Transform>();
        }

        GameObject(const GameObject& _obj) = delete;

        GameObject(GameObject&& _obj);

        virtual ~GameObject() {
            if (mParent)
                mParent->RemoveChild(this);

            for (auto it = mChildren.begin(); it != mChildren.end();) {
                delete (*it++);
            }


            for (auto comp : mComponents)
                delete comp;

            RemoveGameObj(this);
        }

        GameObject& operator=(const GameObject& _obj) = delete;

        GameObject& operator=(GameObject&& _obj);

        /**
         * \brief Add a Component of type _Ty to this GameObject, using default constructor to create the Component
         * \return The pointer to the Component
         */
        template<typename _Ty>
        _Ty* AddComponent();

        /**
         * \brief Add Component _comp to this GameObject
         * \return The pointer to the Component
         */
        Component* AddComponent(Component* _comp);

        /**
         * \brief Construct a Component of type _Ty and add it to this GameObject
         * \return The pointer to the Component
         */
        template<typename _Ty, typename... Args>
        _Ty* AddComponent(Args&& ..._args);

        /**
         * \brief Get the pointer to the Component of type _Ty that attached to this GameObject
         * \return The pointer to the Component, return nullptr if not found
         */
        template<typename T>
        T* GetComponent();

        /**
         * \brief Get all Components of type _Ty that attached to this GameObject
         * \return A vector containing pointers of Components that been found
         */
        template<typename _Ty>
        std::vector<_Ty*> GetComponents();

        /**
         * \brief Get Component of type _Ty in children of this GameObject using DFS
         */
        template<typename _Ty>
        _Ty* GetComponentInChildren();

        /**
         * \brief Remove Component that _comp points to,
         * do nothing if _comp isn't attached to this GameObject
         */
        void RemoveComponent(Component* _comp);

        /**
         * \brief Add _obj as a child of this GameObject
         */
        void AddChild(GameObject* _obj) {
            if (_obj->mParent == this)
                return;

            if (_obj->mParent)
                _obj->mParent->RemoveChild(_obj);

            _obj->mParent = this;
            mChildren.insert(_obj);
        }

        /**
         * \brief Remove a child that _obj points to from this GameObject
         */
        void RemoveChild(GameObject* _obj) {
            if (_obj->mParent != this)
                return;

            _obj->mParent = nullptr;
            mChildren.erase(std::find(mChildren.begin(), mChildren.end(), _obj));
        }

        /**
         * \brief Set the active state this GameObject
         */
        void SetActive(const bool _active) {
            mActive = _active;
        }

        /**
         * \brief Check if this GameObject is active
         */
        bool IsActive() const {
            return mActive;
        }

        /**
         * \brief Get the index of the layer that this GameObject belongs to
         */
        LayerIndex GetLayer() const {
            return mLayer;
        }

        /**
         * \brief Set the index of the layer that this GameObject belongs to
         */
        void SetLayer(const LayerIndex _index) {
            mLayer = _index;
        }

        /**
         * \brief Set a tag to this GameObject
         */
        void SetTag(const Tag& _tag) {
            mTag = _tag;
        }

        /**
         * \brief Get the tag attached to this GameObject
         */
        const Tag& GetTag() const {
            return mTag;
        }

        void SetModelRef(Resource::ResourceRef _ref) {
            mModeRef = _ref;
        }

        Resource::ResourceRef GetModelRef() const {
            return mModeRef.value_or(nullptr);
        }

    protected:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;
        std::optional<Resource::ResourceRef> mModeRef;

        bool mActive = true;
        LayerIndex mLayer = 0;

        Tag mTag;

        // static
    public:
        /**
         * \brief Get a GameObject named _name
         * \return The Pointer to the GameObject, return nullptr if not found
         */
        static GameObject* Find(const std::string& _name);

        /**
         * \brief Get all GameObjects with tag _tag
         * \return A vector containing pointers to GameObjects
         */
        static std::vector<GameObject*> FindGameObjsWithTag(const Tag& _tag);

        /**
         * \brief Get a GameObject with tag _tag
         * \return The pointer to the first GameObject been found,
         * return nullptr if not found
         */
        static GameObject* FindGameObjWithTag(const Tag& _tag);

    private:
        /**
         * \brief Add GameObject _obj points to to global GameObject list
         */
        static void AddGameObj(GameObject* _obj) {
            mGameObjList.push_back(_obj);
        }

        /**
         * \brief Remove the GameObject _obj points to fom global GameObject list
         */
        static void RemoveGameObj(GameObject* _obj) {
            const auto it = std::find(mGameObjList.begin(), mGameObjList.end(), _obj);
            if (it == mGameObjList.end())
                return;
            mGameObjList.erase(it);
        }

        static std::vector<GameObject*> mGameObjList;
    };


    template<typename _Ty>
    inline _Ty * GameObject::AddComponent() {
        // if type _Ty isn't derived from Component
        _Ty* t = reinterpret_cast<_Ty*>(1);
        if (!dynamic_cast<Component*>(t))
            throw ComponentCastError();

        t = new _Ty();
        t->setGameObj(this);
        mComponents.insert(t);
        return t;
    }

    template<typename T, typename ...Args>
    inline T * GameObject::AddComponent(Args && ..._args) {
        // if type _Ty isn't derived from Component
        T* t = reinterpret_cast<T*>(1);
        if (!dynamic_cast<Component*>(t))
            throw ComponentCastError();

        t = new T(std::forward<Args>(_args)...);
        t->setGameObj(this);
        mComponents.insert(t);
        return t;
    }

    template<typename T>
    inline T * GameObject::GetComponent() {
        T* result;

        for (auto comp : mComponents) {
            if ((result = dynamic_cast<T*>(comp)))
                return result;
        }

        return nullptr;
    }

    template<typename T>
    inline std::vector<T*> GameObject::GetComponents() {
        std::vector<T*> results;
        T* result;

        for (auto comp : mComponents) {
            if ((result = dynamic_cast<T*>(comp)))
                results.push_back(result);
        }

        return results;
    }

    template<typename T>
    inline T * GameObject::GetComponentInChildren() {

        T* ptr = GetComponent<T>();

        if (!ptr) {
            for (auto child : mChildren) {
                if ((ptr = child->GetComponentInChildren<T>()))
                    return ptr;
            }
        }

        return nullptr;
    }


}

#endif

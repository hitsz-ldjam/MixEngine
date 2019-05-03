#pragma once

#ifndef _MX_GAME_OBJECT_H_
#define _MX_GAME_OBJECT_H_

#include "MxException.hpp"
#include "MxObject.h"
#include "MxTransform.h"
#include <vector>
#include <set>
#include <type_traits>


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
                mParent->removeChild(this);

            for (auto it = mChildren.begin(); it != mChildren.end();) {
                delete (*it++);
            }
                

            for (auto comp : mComponents)
                delete comp;

            RemoveGameObj(this);
        }

        GameObject& operator=(const GameObject& obj) = delete;
        GameObject& operator=(GameObject&& _obj);

        template<typename T>
        T* AddComponent();

        Component* AddComponent(Component* _comp);

        template<typename T, typename... Args>
        T* AddComponent(Args&& ..._args);

        template<typename T>
        T* GetComponent();

        template<typename T>
        std::vector<T*> GetComponents();

        template<typename T>
        T* GetComponentInChildren();

        void removeComponent(Component* comp);

        void addChild(GameObject* obj) {
            if (obj->mParent == this)
                return;

            if (obj->mParent)
                obj->mParent->removeChild(obj);

            obj->mParent = this;
            mChildren.insert(obj);
        }

        void removeChild(GameObject* obj) {
            if (obj->mParent != this)
                return;

            obj->mParent = nullptr;
            mChildren.erase(std::find(mChildren.begin(), mChildren.end(), obj));
        }

        void setActive(bool active) {
            mActive = active;
        }

        bool isActive() const {
            return mActive;
        }

        LayerIndex getLayer() const {
            return mLayer;
        }

        void SetLayer(const LayerIndex _index) {
            // todo complete this when we have scene manager
            mLayer = _index;
        }

        void SetTag(const Tag& _tag) {
            mTag = _tag;
        }

        const Tag& GetTag() const {
            return mTag;
        }

    protected:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;

        bool mActive = true;
        LayerIndex mLayer = 0;

        Tag mTag;

        // static
    public:
        static GameObject* Find(const std::string& _name);
        static std::vector<GameObject*> FindGameObjsWithTag(const Tag& _tag);
        static GameObject* FindGameObjWithTag(const Tag& _tag);

    private:
        static void AddGameObj(GameObject* _obj) {
            mGameObjList.push_back(_obj);
        }

        static void RemoveGameObj(GameObject* _obj) {
            const auto it = std::find(mGameObjList.begin(), mGameObjList.end(), _obj);
            if (it == mGameObjList.end())
                return;
            mGameObjList.erase(it);
        }

        static std::vector<GameObject*> mGameObjList;
    };


    template<typename T>
    inline T * GameObject::AddComponent() {
        // if type T isn't derived from Component
        T* t = reinterpret_cast<T*>(1);
        if (!dynamic_cast<Component*>(t))
            throw ComponentCastError();

        t = new T();
        t->setGameObj(this);
        mComponents.insert(t);
        return t;
    }

    template<typename T, typename ...Args>
    inline T * GameObject::AddComponent(Args && ..._args) {
        // if type T isn't derived from Component
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

#pragma once

#ifndef _MX_GAME_OBJECT_H_
#define _MX_GAME_OBJECT_H_

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
            addGameObj(this);
            addComponent<Transform>();
        }

        GameObject(const GameObject& obj) = delete;
        GameObject(GameObject&& obj);

        virtual ~GameObject() {
            if (mParent)
                mParent->removeChild(this);

            for (auto it = mChildren.begin(); it != mChildren.end();) {
                delete (*it++);
            }
                

            for (auto comp : mComponents)
                delete comp;

            removeGameObj(this);
        }

        GameObject& operator=(const GameObject& obj) = delete;
        GameObject& operator=(GameObject&& obj);

        template<typename T>
        T* addComponent();

        Component* addComponent(Component* comp);

        template<typename T, typename... Args>
        T* addComponent(Args&& ...args);

        template<typename T>
        T* getComponent();

        template<typename T>
        std::vector<T*> getComponents();

        template<typename T>
        T* getComponentInChildren();

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

        void setLayer(LayerIndex index) {
            // todo complete this when we have scene manager
            mLayer = index;
        }

        void setTag(const Tag& tag) {
            mTag = tag;
        }

        const Tag& getTag() const {
            return mTag;
        }

        const std::string& getName() const {
            return mName;
        }

    protected:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;

        bool mActive = true;
        LayerIndex mLayer = 0;

        std::string mName;
        Tag mTag;

        // static
    public:
        static GameObject* find(const std::string& name);
        static std::vector<GameObject*> findGameObjsWithTag(const Tag& tag);
        static GameObject* findGameObjWithTag(const Tag& tag);

    private:
        static void addGameObj(GameObject* obj) {
            mGameObjList.push_back(obj);
        }

        static void removeGameObj(GameObject* obj) {
            auto it = std::find(mGameObjList.begin(), mGameObjList.end(), obj);
            if (it == mGameObjList.end())
                return;
            mGameObjList.erase(it);
        }

        static std::vector<GameObject*> mGameObjList;
    };


    template<typename T>
    inline T * GameObject::addComponent() {
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
    inline T * GameObject::addComponent(Args && ...args) {
        // if type T isn't derived from Component
        T* t = reinterpret_cast<T*>(1);
        if (!dynamic_cast<Component*>(t))
            throw ComponentCastError();

        t = new T(std::forward<Args>(args)...);
        t->setGameObj(this);
        mComponents.insert(t);
        return t;
    }

    template<typename T>
    inline T * GameObject::getComponent() {
        T* result;

        for (auto comp : mComponents) {
            if (result = dynamic_cast<T*>(comp))
                return result;
        }

        return nullptr;
    }

    template<typename T>
    inline std::vector<T*> GameObject::getComponents() {
        std::vector<T*> results;
        T* result;

        for (auto comp : mComponents) {
            if (result = dynamic_cast<T*>(comp))
                results.push_back(result);
        }

        return results;
    }

    template<typename T>
    inline T * GameObject::getComponentInChildren() {

        T* ptr;

        ptr = getComponent(T)();

        if (!ptr) {
            for (auto child : mChildren) {
                if (ptr = child->getComponentInChildren<T>())
                    return ptr;
            }
        }
    }


}

#endif

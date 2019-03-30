#pragma once

#ifndef _MX_GAME_OBJECT_H_
#define _MX_GAME_OBJECT_H_

#include "MxObject.h"
#include "MxComponent.h"
#include "MxTransform.h"
#include <vector>
#include <set>


namespace Mix {
    // todo: finish this class
    class GameObject : public Object {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        GameObject() :mParent(nullptr) {
            addComponent<Transform>();
        }

        GameObject(const GameObject& obj);

        GameObject(GameObject&& obj);

        virtual ~GameObject() {
            if (mParent)
                mParent->removeChild(this);

            for (auto child : mChildren)
                removeChild(child);

            for (auto comp : mComponents)
                delete comp;
        }

        GameObject& operator=(const GameObject& obj);
        GameObject& operator=(GameObject&& obj);

        template<typename T>
        T* addComponent();

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
    };


    template<typename T>
    inline T * GameObject::addComponent() {
        T* t = new T();
        Component* comp = dynamic_cast<Component*>(t);

        // if type T isn't derived from Component
        if (!comp)
            throw ComponentCastError();

        mComponents.insert(comp);
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

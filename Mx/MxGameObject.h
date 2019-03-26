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
        T* addComponent() {
            T* t = new T();
            Component* comp = dynamic_cast<Component*>(t);

            // if type T isn't derived from Component
            if (!comp)
                throw ComponentCastError();

            mComponents.insert(comp);
            return t;
        }

        template<typename T>
        T* getComponent() {
            T* result;

            for (auto comp : mComponents) {
                if (result = dynamic_cast<T*>(comp))
                    return result;
            }

            return nullptr;
        }

        template<typename T>
        std::vector<T*> getComponents() {
            std::vector<T*> results;
            T* result;

            for (auto comp : mComponents) {
                if (result = dynamic_cast<T*>(comp))
                    results.push_back(result);
            }

            return results;
        }

        template<typename T>
        T* getComponentInChildren() {
            T* ptr;

            ptr = getComponent(T)();

            if (!ptr) {
                for (auto child : mChildren) {
                    if (ptr = child->getComponentInChildren<T>())
                        return ptr;
                }
            }
        }

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

    private:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;
    };


}

#endif

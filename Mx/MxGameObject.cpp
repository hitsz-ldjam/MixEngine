#include "MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NoCreateFunc(GameObject, Object);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject);

    GameObject::GameObject(const GameObject & obj) {
        *this = obj;
    }

    GameObject::GameObject(GameObject && obj) {
        *this = std::move(obj);
    }

    GameObject & GameObject::operator=(const GameObject & obj) {
        mParent = nullptr;

        for (auto child : obj.mChildren) {
            mChildren.insert(new GameObject(*child));
        }

        for (auto comp : obj.mComponents) {
            Component* p = comp->copy();
            p->setGameObj(this);
            mComponents.insert(p);
        }

        return *this;
    }

    GameObject & GameObject::operator=(GameObject && obj) {
        mParent = obj.mParent;
        mChildren = std::move(obj.mChildren);
        mComponents = std::move(obj.mComponents);

        for (auto child : mChildren) {
            this->addChild(child);
        }

        for (auto comp : mComponents) {
            comp->setGameObj(this);
        }

        obj.mParent = nullptr;
        return *this;
    }

}
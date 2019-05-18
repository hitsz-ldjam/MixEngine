#include "MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(GameObject, Object);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject);

    std::vector<GameObject*> GameObject::mGameObjList;

    GameObject::GameObject(GameObject && _other)noexcept :Base(std::move(_other)) {
        mParent = _other.mParent;
        mChildren = std::move(_other.mChildren);
        mComponents = std::move(_other.mComponents);
        mActive = _other.mActive;
        mLayer = _other.mLayer;
        mTag = std::move(_other.mTag);

        for (auto child : mChildren) {
            this->addChild(child);
        }

        for (auto comp : mComponents) {
            comp->setGameObj(this);
        }

        _other.mParent = nullptr;
    }

    GameObject & GameObject::operator=(GameObject && _other) noexcept {
        mParent = _other.mParent;
        mChildren = std::move(_other.mChildren);
        mComponents = std::move(_other.mComponents);
        mActive = _other.mActive;
        mLayer = _other.mLayer;
        mTag = std::move(_other.mTag);

        for (auto child : mChildren) {
            this->addChild(child);
        }

        for (auto comp : mComponents) {
            comp->setGameObj(this);
        }

        _other.mParent = nullptr;
        Base::operator=(std::move(_other));

        return *this;
    }

    Component * GameObject::addComponent(Component * _comp) {
        if (!_comp)
            return nullptr;

        _comp->setGameObj(this);
        mComponents.insert(_comp);
        return _comp;
    }

    void GameObject::removeComponent(Component * _comp) {
        auto it = std::find(mComponents.begin(), mComponents.end(), _comp);
        if (it != mComponents.end()) {
            mComponents.erase(it);
        }
    }

    GameObject * GameObject::Find(const std::string & _name) {
        auto it = std::find_if(mGameObjList.begin(), mGameObjList.end(), [&](const GameObject* obj) ->bool {return obj->mName == _name; });
        if (it == mGameObjList.end())
            return nullptr;

        return *it;
    }

    std::vector<GameObject*> GameObject::FindGameObjsWithTag(const Tag & _tag) {
        std::vector<GameObject*> results;
        std::for_each(mGameObjList.begin(), mGameObjList.end(), [&](GameObject* obj)mutable {if (obj->mTag == _tag)results.push_back(obj); });
        return results;
    }

    GameObject * GameObject::FindGameObjWithTag(const Tag & _tag) {
        auto it = std::find_if(mGameObjList.begin(), mGameObjList.end(), [&](const GameObject* actor) ->bool {return actor->mTag == _tag; });
        if (it == mGameObjList.end())
            return nullptr;

        return *it;
    }

}
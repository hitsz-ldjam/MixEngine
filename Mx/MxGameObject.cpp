#include "MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NoCreateFunc(GameObject, Object);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject);

    std::vector<GameObject*> GameObject::mGameObjList;

    GameObject::GameObject(GameObject && _obj) noexcept {
        *this = std::move(_obj);
    }

    GameObject & GameObject::operator=(GameObject && _obj) {
        mParent = _obj.mParent;
        mChildren = std::move(_obj.mChildren);
        mComponents = std::move(_obj.mComponents);
        mActive = _obj.mActive;
        mName = _obj.mName;
        mLayer = _obj.mLayer;

        for (auto child : mChildren) {
            this->AddChild(child);
        }

        for (auto comp : mComponents) {
            comp->SetGameObj(this);
        }

        _obj.mParent = nullptr;
        return *this;
    }

    Component * GameObject::AddComponent(Component * _comp) {
        if (!_comp)
            return nullptr;

        _comp->SetGameObj(this);
        mComponents.insert(_comp);
        return _comp;
    }

    void GameObject::RemoveComponent(Component * _comp) {
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
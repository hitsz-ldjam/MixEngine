#include "MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NoCreateFunc(GameObject, Object);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject);

    std::vector<GameObject*> GameObject::mGameObjList;

    /*GameObject::GameObject(const GameObject & obj) {
        *this = obj;
    }*/

    GameObject::GameObject(GameObject && obj) {
        *this = std::move(obj);
    }

   /* GameObject & GameObject::operator=(const GameObject & obj) {
        mParent = nullptr;
        mActive = obj.mActive;
        mName = obj.mName;
        mLayer = obj.mLayer;

        for (auto child : obj.mChildren) {
            mChildren.insert(new GameObject(*child));
        }

        for (auto comp : obj.mComponents) {
            Component* p = comp->copy();
            p->setGameObj(this);
            mComponents.insert(p);
        }

        return *this;
    }*/

    GameObject & GameObject::operator=(GameObject && obj) {
        mParent = obj.mParent;
        mChildren = std::move(obj.mChildren);
        mComponents = std::move(obj.mComponents);
        mActive = obj.mActive;
        mName = obj.mName;
        mLayer = obj.mLayer;

        for (auto child : mChildren) {
            this->addChild(child);
        }

        for (auto comp : mComponents) {
            comp->setGameObj(this);
        }

        obj.mParent = nullptr;
        return *this;
    }

    Component * GameObject::addComponent(Component * comp) {
        if (!comp)
            return nullptr;

        comp->setGameObj(this);
        mComponents.insert(comp);
        return comp;
    }

    void GameObject::removeComponent(Component * comp) {
        auto it = std::find(mComponents.begin(), mComponents.end(), comp);
        if (it != mComponents.end()) {
            mComponents.erase(it);
        }
    }

    GameObject * GameObject::find(const std::string & name) {
        auto it = std::find_if(mGameObjList.begin(), mGameObjList.end(), [&](const GameObject* obj) ->bool {return obj->mName == name; });
        if (it == mGameObjList.end())
            return nullptr;

        return *it;
    }

    std::vector<GameObject*> GameObject::findGameObjsWithTag(const Tag & tag) {
        std::vector<GameObject*> results;
        std::for_each(mGameObjList.begin(), mGameObjList.end(), [&](GameObject* obj)mutable {if (obj->mTag == tag)results.push_back(obj); });
        return results;
    }

    GameObject * GameObject::findGameObjWithTag(const Tag & tag) {
        auto it = std::find_if(mGameObjList.begin(), mGameObjList.end(), [&](const GameObject* actor) ->bool {return actor->mTag == tag; });
        if (it == mGameObjList.end())
            return nullptr;

        return *it;
    }

}
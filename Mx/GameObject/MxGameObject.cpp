#include "MxGameObject.h"
#include <algorithm>

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(GameObject, Object)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject)

    std::vector<GameObject*> GameObject::sGameObjList;

    GameObject::GameObject(): mParent(nullptr),
                              mActiveInHierarchy(true),
                              mActiveSelf(true),
                              mIsStatic(false),
                              mLayer(0),
                              mTransform(nullptr),
                              mScene(nullptr) {}

    GameObject::GameObject(const GameObjectCreateInfo& _info): mParent(nullptr),
                                                                     mActiveInHierarchy(true),
                                                                     mActiveSelf(true),
                                                                     mIsStatic(_info.isStatic),
                                                                     mTag(_info.tag),
                                                                     mLayer(_info.layer),
                                                                     mTransform(addComponent<Transform>()),
                                                                     mScene(nullptr) {
        mName = _info.name;
        setActive(mActiveSelf);
        AddGameObject(this);
    }

    GameObject::GameObject(GameObject&& _obj) noexcept : Object(std::move(_obj)),
                                                         mParent(_obj.mParent),
                                                         mChildren(std::move(_obj.mChildren)),
                                                         mComponents(std::move(_obj.mComponents)),
                                                         mBehaviours(std::move(_obj.mBehaviours)),
                                                         mActiveInHierarchy(_obj.mActiveInHierarchy),
                                                         mActiveSelf(_obj.mActiveSelf),
                                                         mIsStatic(_obj.mIsStatic),
                                                         mTag(std::move(_obj.mTag)),
                                                         mLayer(_obj.mLayer),
                                                         mTransform(_obj.mTransform),
                                                         mScene(_obj.mScene) {
        for(auto child : mChildren)
            addChild(child);

        for(auto component : mComponents)
            component->setGameObject(this);

        AddGameObject(this);
    }

    GameObject& GameObject::operator=(GameObject&& _obj) noexcept {
        Object::operator=(_obj);
        mParent = _obj.mParent;
        mChildren = std::move(_obj.mChildren);
        mComponents = std::move(_obj.mComponents);
        mBehaviours = std::move(_obj.mBehaviours);
        mActiveInHierarchy = _obj.mActiveInHierarchy;
        mActiveSelf = _obj.mActiveSelf;
        mIsStatic = _obj.mIsStatic;
        mTag = std::move(_obj.mTag);
        mLayer = _obj.mLayer;
        mTransform = _obj.mTransform;
        mScene = _obj.mScene;

        for(auto child : mChildren)
            addChild(child);

        for(auto component : mComponents)
            component->setGameObject(this);

        return *this;
    }

    GameObject::~GameObject() {
        if(mParent)
            mParent->removeChild(this);

        for(auto child : mChildren)
            delete child;

        for(auto component : mComponents)
            delete component;

        RemoveGameObject(this);
    }

    Component* GameObject::addComponent(Component* _comp) {
        if(!_comp)
            return nullptr;

        _comp->setGameObject(this);
        mComponents.insert(_comp);
        if(auto b = dynamic_cast<Behaviour*>(_comp))
            mBehaviours.insert(b);
        return _comp;
    }

    void GameObject::removeComponent(Component* _comp) {
        mComponents.erase(_comp);
        if(auto b = dynamic_cast<Behaviour*>(_comp))
            mBehaviours.erase(b);
    }

    void GameObject::addChild(GameObject* _obj) {
        if(!_obj || _obj->mParent == this)
            return;

        if(_obj->mParent)
            _obj->mParent->removeChild(_obj);

        _obj->mParent = this;
        _obj->setActive(_obj->mActiveSelf);
        mChildren.insert(_obj);
    }

    void GameObject::removeChild(GameObject* _obj) {
        if(!_obj || _obj->mParent != this)
            return;

        _obj->mParent = nullptr;
        mChildren.erase(_obj);
    }

    void GameObject::setActive(const bool _active) {
        mActiveSelf = _active;
        const bool activedInHierarchy = mActiveInHierarchy;

        if(mActiveSelf && mParent)
            mActiveInHierarchy = mParent->mActiveInHierarchy;
        else
            mActiveInHierarchy = mActiveSelf;

        if(mActiveInHierarchy == activedInHierarchy)
            return;

        for(auto child : mChildren)
            child->setActive(child->mActiveSelf);

        if(mActiveSelf) {
            // todo: call OnEnable() of Scripts
        }
        else {
            // todo: call OnDisable() of Scripts
            // todo: disable Update() of Scripts
        }
    }

    GameObject* GameObject::Find(const std::string& _name) {
        const auto it = std::find_if(sGameObjList.begin(), sGameObjList.end(), [&](const GameObject* _obj) -> bool { return _obj->mName == _name; });
        if(it == sGameObjList.end())
            return nullptr;

        return *it;
    }

    std::vector<GameObject*> GameObject::FindGameObjectsWithTag(const Tag& _tag) {
        std::vector<GameObject*> results;
        std::for_each(sGameObjList.begin(), sGameObjList.end(), [&](GameObject* _obj)mutable { if(_obj->mTag == _tag)results.push_back(_obj); });
        return results;
    }

    GameObject* GameObject::FindGameObjectWithTag(const Tag& _tag) {
        const auto it = std::find_if(sGameObjList.begin(), sGameObjList.end(), [&](const GameObject* _obj) -> bool { return _obj->mTag == _tag; });
        if(it == sGameObjList.end())
            return nullptr;

        return *it;
    }

}

#include "MxGameObject.h"
#include <algorithm>

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(GameObject, Object)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject)

    std::vector<GameObject*> GameObject::sGameObjList;

    GameObject::GameObject() : mParent(nullptr),
                               mTransform(nullptr),
                               mLayer(0),
                               mIsStatic(false),
                               mActiveSelf(true),
                               mActiveInHierarchy(true),
                               mScene(nullptr) {}

    GameObject::GameObject(std::string _name,
                           Tag _tag,
                           const LayerIndex _layerIndex,
                           const bool _isStatic) : mParent(nullptr),
                                                   mTransform(addComponent<Transform>()),
                                                   mTag(std::move(_tag)),
                                                   mLayer(_layerIndex),
                                                   mIsStatic(_isStatic),
                                                   mActiveSelf(true),
                                                   mActiveInHierarchy(true),
                                                   mScene(nullptr) {
        mName = std::move(_name);
        setActive(mActiveSelf);
        AddGameObject(this);
    }

    GameObject::GameObject(const GameObjectConInfo& _info)
        : GameObject(_info.name, _info.tag, _info.layerIndex, _info.isStatic) {}

    GameObject::GameObject(GameObject&& _obj) noexcept : Object(std::move(_obj)),
                                                         mParent(_obj.mParent),
                                                         mChildren(std::move(_obj.mChildren)),
                                                         mComponents(std::move(_obj.mComponents)),
                                                         mBehaviours(std::move(_obj.mBehaviours)),
                                                         mTransform(_obj.mTransform),
                                                         mTag(std::move(_obj.mTag)),
                                                         mLayer(_obj.mLayer),
                                                         mIsStatic(_obj.mIsStatic),
                                                         mActiveSelf(_obj.mActiveSelf),
                                                         mActiveInHierarchy(_obj.mActiveInHierarchy),
                                                         mScene(_obj.mScene) {
        for(auto child : mChildren)
            addChild(child);

        for(auto component : mComponents)
            component->setGameObject(this);

        // todo: remove previous object from Scene and add the new one to Scene

        AddGameObject(this);
    }

    GameObject& GameObject::operator=(GameObject&& _obj) noexcept {
        Object::operator=(_obj);
        mParent = _obj.mParent;
        mChildren = std::move(_obj.mChildren);
        mComponents = std::move(_obj.mComponents);
        mBehaviours = std::move(_obj.mBehaviours);
        mTransform = _obj.mTransform;
        mTag = std::move(_obj.mTag);
        mLayer = _obj.mLayer;
        mIsStatic = _obj.mIsStatic;
        mActiveSelf = _obj.mActiveSelf;
        mActiveInHierarchy = _obj.mActiveInHierarchy;
        mScene = _obj.mScene;

        for(auto child : mChildren)
            addChild(child);

        for(auto component : mComponents)
            component->setGameObject(this);

        // todo: remove previous object from Scene and add the new one to Scene

        return *this;
    }

    GameObject::~GameObject() {
        if(mParent)
            mParent->removeChild(this);

        for(auto child : mChildren)
            delete child;

        for(auto component : mComponents)
            delete component;

        // todo: remove from Scene

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

    void GameObject::addToScene(Scene* _scene) {
        mScene = _scene;
    }

    void GameObject::removeFromScene(Scene* _scene) {
        mScene = nullptr;
    }

    GameObject* GameObject::Find(const std::string& _name) {
        const auto it = std::find_if(sGameObjList.begin(), sGameObjList.end(), [&name = _name](const GameObject* _obj) -> bool { return _obj->mName == name; });
        if(it == sGameObjList.end())
            return nullptr;

        return *it;
    }

    std::vector<GameObject*> GameObject::FindGameObjectsWithTag(const Tag& _tag) {
        std::vector<GameObject*> results;
        std::for_each(sGameObjList.begin(),
                      sGameObjList.end(),
                      [&tag = _tag, &results = results](auto* _obj) mutable {
                          if(_obj->mTag == tag)
                              results.push_back(_obj);
                      });
        return results;
    }

    GameObject* GameObject::FindGameObjectWithTag(const Tag& _tag) {
        const auto it = std::find_if(sGameObjList.begin(),
                                     sGameObjList.end(),
                                     [&tag = _tag](auto* _obj) { return _obj->mTag == tag; });
        if(it == sGameObjList.end())
            return nullptr;
        return *it;
    }

}

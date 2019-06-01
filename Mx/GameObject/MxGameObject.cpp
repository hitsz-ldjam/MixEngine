#include "MxGameObject.h"
#include <utility>

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(GameObject, Object)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(GameObject)

    std::vector<GameObject*> GameObject::sGameObjList;

    GameObject::GameObject(const std::string& _name, Tag _tag, const LayerIndex _layer, const bool _isStatic)
        : mParent(nullptr),
          mActiveInHierarchy(true),
          mActiveSelf(true),
          mIsStatic(_isStatic),
          mLayer(_layer),
          mTag(std::move(_tag)),
          mTransform(nullptr) {
        mName = _name;
        AddGameObject(this);
        mTransform = addComponent<Transform>();
        setActive(mActiveSelf);
    }

    GameObject::GameObject(GameObject&& _obj) noexcept: Object(std::move(_obj)) {
        mParent = _obj.mParent;
        mChildren = std::move(_obj.mChildren);
        mComponents = std::move(_obj.mComponents);
        mActiveInHierarchy = _obj.mActiveInHierarchy;
        mActiveSelf = _obj.mActiveSelf;
        mIsStatic = _obj.mIsStatic;
        mLayer = _obj.mLayer;
        mTag = std::move(_obj.mTag);
        mTransform = getComponent<Transform>();

        for(auto child : mChildren) {
            this->addChild(child);
        }

        for(auto comp : mComponents) {
            comp->setGameObject(this);
        }

        setActive(mActiveSelf);
    }

    GameObject& GameObject::operator=(GameObject&& _obj) noexcept {
        mParent = _obj.mParent;
        mChildren = std::move(_obj.mChildren);
        mComponents = std::move(_obj.mComponents);
        mActiveInHierarchy = _obj.mActiveInHierarchy;
        mActiveSelf = _obj.mActiveSelf;
        mIsStatic = _obj.mIsStatic;
        mLayer = _obj.mLayer;
        mTag = std::move(_obj.mTag);
        mTransform = getComponent<Transform>();

        for(auto child : mChildren) {
            this->addChild(child);
        }

        for(auto comp : mComponents) {
            comp->setGameObject(this);
        }

        Object::operator=(std::move(_obj));

        setActive(mActiveSelf);

        return *this;
    }

    GameObject::~GameObject() {
        if(mParent)
            mParent->removeChild(this);

        for(auto it = mChildren.begin(); it != mChildren.end();)
            delete (*it++);

        for(auto comp : mComponents)
            delete comp;

        RemoveGameObject(this);
    }

    Component* GameObject::addComponent(Component* _comp) {
        if(!_comp)
            return nullptr;

        _comp->setGameObject(this);
        mComponents.insert(_comp);
        return _comp;
    }

    void GameObject::removeComponent(Component* _comp) {
        auto it = std::find(mComponents.begin(), mComponents.end(), _comp);
        if(it != mComponents.end()) {
            mComponents.erase(it);
        }
    }

    void GameObject::addChild(GameObject* _obj) {
        if(_obj->mParent == this)
            return;

        if(_obj->mParent)
            _obj->mParent->removeChild(_obj);

        _obj->mParent = this;
        _obj->setActive(_obj->mActiveSelf);
        mChildren.insert(_obj);
    }

    void GameObject::removeChild(GameObject* _obj) {
        if(_obj->mParent != this)
            return;

        _obj->mParent = nullptr;
        mChildren.erase(std::find(mChildren.begin(), mChildren.end(), _obj));
    }

    std::vector<GameObject*> GameObject::getAllChildren() const {
        return std::vector<GameObject*>(mChildren.begin(), mChildren.end());
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

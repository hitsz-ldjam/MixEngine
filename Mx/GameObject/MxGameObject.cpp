#include "MxGameObject.h"
#include <algorithm>
#include "../Scene/MxSceneManager.hpp"
#include "../Component/MxComponent.h"
#include "../Log/MxLog.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(GameObject, Object);

   /* GameObject::GameObject() : mParent(nullptr),
        mTransform(nullptr),
        mLayer(0),
        mIsStatic(false),
        mActiveSelf(true),
        mActiveInHierarchy(true),
        mScene(nullptr) {
    }

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
    }*/

    //GameObject::GameObject(GameObject&& _obj) noexcept : SceneObject(std::move(_obj)),
    //    mParent(_obj.mParent),
    //    mChildren(std::move(_obj.mChildren)),
    //    mComponents(std::move(_obj.mComponents)),
    //    mBehaviours(std::move(_obj.mBehaviours)),
    //    mTransform(_obj.mTransform),
    //    mTag(std::move(_obj.mTag)),
    //    mLayer(_obj.mLayer),
    //    mIsStatic(_obj.mIsStatic),
    //    mActiveSelf(_obj.mActiveSelf),
    //    mActiveInHierarchy(_obj.mActiveInHierarchy),
    //    mScene(_obj.mScene) {
    //    for (auto child : mChildren)
    //        addChild(child);

    //    for (auto component : mComponents)
    //        component->setGameObject(this);

    //    // todo: remove previous object from Scene and add the new one to Scene

    //    AddGameObject(this);
    //}

    //GameObject& GameObject::operator=(GameObject&& _obj) noexcept {
    //    Object::operator=(_obj);
    //    mParent = _obj.mParent;
    //    mChildren = std::move(_obj.mChildren);
    //    mComponents = std::move(_obj.mComponents);
    //    mBehaviours = std::move(_obj.mBehaviours);
    //    mTransform = _obj.mTransform;
    //    mTag = std::move(_obj.mTag);
    //    mLayer = _obj.mLayer;
    //    mIsStatic = _obj.mIsStatic;
    //    mActiveSelf = _obj.mActiveSelf;
    //    mActiveInHierarchy = _obj.mActiveInHierarchy;
    //    mScene = _obj.mScene;

    //    for (auto child : mChildren)
    //        addChild(child);

    //    for (auto component : mComponents)
    //        component->setGameObject(this);

    //    // todo: remove previous object from Scene and add the new one to Scene

    //    return *this;
    //}

    //GameObject::~GameObject() {
    //    if (mParent)
    //        mParent->removeChild(this);

    //    for (auto child : mChildren)
    //        delete child;

    //    for (auto component : mComponents)
    //        delete component;

    //    // todo: remove from Scene

    //    RemoveGameObject(this);
    //}

    GameObject::~GameObject() {
        if (!mThisHandle.isDestroyed()) {
            MX_LOG_WARNING("Object [%1%] is being deleted without being destroyed first?", mName);
            destroyInternal(mThisHandle, true);
        }
    }

    HGameObject GameObject::Create(const std::string& _name, Tag _tag, const LayerIndex _layerIndex, Flags<GameObjectFlags> _flags) {
        return CreateInternal(std::move(_name), std::move(_tag), _layerIndex, _flags, SceneManager::Get()->getActiveScene());
    }

    void GameObject::destroy(bool _immediate) {
        if (mParent != nullptr) {
            if (!mParent.isDestroyed())
                mParent->removeChild(mThisHandle);

            mParent = nullptr;
        }

        destroyInternal(mThisHandle, _immediate);
    }

    void GameObject::destroyInternal(SceneObjectHandleBase& _handle, bool _immediate) {
        if (_immediate) {
            // Remove children
            for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
                (*iter)->destroyInternal(*iter, true);

            mChildren.clear();

            // Remove component
            while (!mComponents.empty()) {
                auto& component = mComponents.back();
                component->destroyInternal(component, true);
                mComponents.erase(mComponents.end() - 1);
            }


            if (mScene)
                mScene->unregisterGameObject(mThisHandle);
            SceneObjectManager::Get()->unregisterObject(_handle);
        }
        else {
            SceneObjectManager::Get()->pushToDestroyQueue(_handle);
        }
    }

    void GameObject::_setFlags(Flags<GameObjectFlags> _flags) {
        mFlags |= _flags;

        for (auto& child : mChildren)
            child->_setFlags(_flags);
    }

    void GameObject::_unsetFlags(Flags<GameObjectFlags> _flags) {
        mFlags &= ~_flags;

        for (auto& child : mChildren)
            child->_unsetFlags(_flags);
    }

    GameObject::GameObject(const std::string& _name, const Tag& _tag, LayerIndex _layerIndex, Flags<GameObjectFlags> _flags)
        :mFlags(_flags), mTag(_tag), mLayer(_layerIndex) {
        addComponent<Transform>();
        mTransform = static_scene_object_cast<Transform>(mComponents[0]);
        setName(_name);
    }

    HGameObject GameObject::CreateInternal(const std::shared_ptr<GameObject>& _gameObject) {
        HGameObject handle = static_scene_object_cast<GameObject>(SceneObjectManager::Get()->registerObject(_gameObject));
        handle->mThisHandle = handle;
        return handle;
    }

    HGameObject GameObject::CreateInternal(const std::string& _name, const Tag& _tag, LayerIndex _layerIndex, Flags<GameObjectFlags> _flags, const std::shared_ptr<Scene>& _scene) {
        auto ptr = std::shared_ptr<GameObject>(new GameObject(_name, _tag, _layerIndex, _flags));
        ptr->mUUID = UUID::RandomUUID();

        HGameObject gameObject = static_scene_object_cast<GameObject>(SceneObjectManager::Get()->registerObject(ptr));
        gameObject->mThisHandle = gameObject;

        if (_scene)
            _scene->registerGameObject(gameObject);

        return gameObject;
    }

    void GameObject::removeComponent(const HComponent& _component, bool _immediate) {
        auto it = std::find(mComponents.begin(), mComponents.end(), _component);

        if (it != mComponents.end()) {
            (*it)->_setIsDestroyed();

            (*it)->destroyInternal(*it, _immediate);

            if (_component->isDerived(Behaviour::GetType()))
                mBehaviours.erase(std::find(mBehaviours.begin(), mBehaviours.end(), static_scene_object_cast<Behaviour>(_component)));

            mComponents.erase(it);
        }

        Log::Warning("Trying to remove a component that isn't attached to this GameObject");
    }

    void GameObject::addAndInitializeComponent(const HComponent& _component) {
        _component->mThisHandle = _component;

        if (_component->mUUID.empty())
            _component->mUUID = UUID::RandomUUID();

        _component->mGameObject = mThisHandle;

        mComponents.push_back(_component);

        if (_component->isDerived(Behaviour::GetType()))
            mBehaviours.push_back(static_scene_object_cast<Behaviour>(_component));
    }

    HComponent GameObject::getComponent(const Rtti& _type) const {
        if (_type.isDerivedFrom(Component::GetType())) {
            for (auto& comp : mComponents) {
                if (comp->getType().isSameType(_type) || comp->getType().isDerivedFrom(_type))
                    return comp;
            }
        }

        return HComponent();
    }

    void GameObject::setParent(const HGameObject& _parent, bool _keepWorldTransform) {
        if (_parent == nullptr || _parent.isDestroyed()) {
            MX_LOG_WARNING("GameObject handle is null or GameObject has been destroyed.");
            return;
        }

        if (mThisHandle == _parent) {
            Log::Warning("Attempting to make a GameObject a child of itself.");
            return;
        }

        if (getParent() == _parent) {
            Log::Warning("Attempting to add a GameObject that is already a child object as a child object.");
            return;
        }

        if (_parent->getParent() == mThisHandle) {
            Log::Warning("Attempting to make a GameObject a child of its child.");
            return;
        }

        Vector3f position;
        Quaternion rotation;
        if (_keepWorldTransform) {
            position = transform().getPosition();
            rotation = transform().getRotation();
        }

        _parent->addChild(mThisHandle);
        setScene(_parent->mScene);

        if (mParent != nullptr)
            mParent->removeChild(mThisHandle);

        if (_keepWorldTransform && mParent != nullptr) {
            transform().setPosition(position);
            transform().setRotation(rotation);
        }


    }

    void GameObject::setScene(const std::shared_ptr<Scene>& _scene) {
        if (mScene == _scene)
            return;
        mScene = _scene;
        mScene->registerGameObject(mThisHandle);
        for (auto& child : mChildren)
            child->setScene(_scene);
    }

    void GameObject::addChild(const HGameObject& _object, bool _keepWorldTransform) {
        mChildren.push_back(_object);
        _object->_setFlags(mFlags);
    }

    HGameObject GameObject::getChild(uint32_t _index) const {
        if (_index >= mChildren.size())
            MX_EXCEPT("Child index out of range.");
        return mChildren[_index];
    }

    void GameObject::removeChild(const HGameObject& _obj) {
        auto found = std::find(mChildren.begin(), mChildren.end(), _obj);

        if (found != mChildren.end()) {
            mChildren.erase(found);
        }
        else {
            MX_EXCEPT("Attempting to remove a child that is not a child of this GameObject");
        }
    }

    int GameObject::indexOfChild(const HGameObject& _child) const {
        for (int i = 0; i < static_cast<int>(mChildren.size()); ++i) {
            if (mChildren[i] == _child)
                return i;
        }
        return -1;
    }

    std::shared_ptr<Scene> GameObject::getScene() const {
        if (mScene)
            return mScene;

        Log::Warning("Attempting to get a scene of a GameObject belongs to no Scene");
        return nullptr;
    }

    HGameObject GameObject::findChild(const std::string& _name, bool _recursive) {
        for (auto& child : mChildren) {
            if (child->getName() == _name)
                return child;
        }

        if (_recursive) {
            for (auto& child : mChildren) {
                auto found = child->findChild(_name, true);
                if (found != nullptr)
                    return found;
            }
        }
        return HGameObject();
    }

    std::vector<HGameObject> GameObject::findChildren(const std::string& _name, bool _recursive) {
        std::vector<HGameObject> results;

        for (auto& child : mChildren) {
            if (child->getName() == _name)
                results.push_back(child);

            if (_recursive) {
                auto found = child->findChildren(_name, _recursive);
                results.insert(results.end(), found.begin(), found.end());
            }
        }
        return results;
    }

    void GameObject::setActive(const bool _active) {
        mActiveSelf = _active;
        const bool activedInHierarchy = mActiveInHierarchy;

        if (mActiveSelf && mParent)
            mActiveInHierarchy = mParent->mActiveInHierarchy;
        else
            mActiveInHierarchy = mActiveSelf;

        if (mActiveInHierarchy == activedInHierarchy)
            return;

        for (auto child : mChildren)
            child->setActive(child->mActiveSelf);

        if (mActiveSelf) {
            // todo: call OnEnable() of Scripts
        }
        else {
            // todo: call OnDisable() of Scripts
            // todo: disable Update() of Scripts
        }
    }

    /*GameObject* GameObject::Find(const std::string& _name) {
        const auto it = std::find_if(sGameObjList.begin(), sGameObjList.end(), [&name = _name](const GameObject* _obj) -> bool { return _obj->mName == name; });
        if (it == sGameObjList.end())
            return nullptr;

        return *it;
    }

    std::vector<GameObject*> GameObject::FindGameObjectsWithTag(const Tag& _tag) {
        std::vector<GameObject*> results;
        std::for_each(sGameObjList.begin(),
                      sGameObjList.end(),
                      [&tag = _tag, &results = results](auto* _obj) mutable {
                          if (_obj->mTag == tag)
                              results.push_back(_obj);
                      });
        return results;
    }

    GameObject* GameObject::FindGameObjectWithTag(const Tag& _tag) {
        const auto it = std::find_if(sGameObjList.begin(),
                                     sGameObjList.end(),
                                     [&tag = _tag](auto* _obj) { return _obj->mTag == tag; });
        if (it == sGameObjList.end())
            return nullptr;
        return *it;
    }*/

}

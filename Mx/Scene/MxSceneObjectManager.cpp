#include "MxSceneObjectManager.h"
#include "MxSceneObject.h"
#include "MxSceneObjectHandle.h"
#include "../../MixEngine.h"

namespace Mix {
    SceneObjectManager* SceneObjectManager::Get() {
        return MixEngine::Instance().getModule<SceneObjectManager>();
    }

    void SceneObjectManager::update() {
    }

    void SceneObjectManager::lateUpdate() {
    }

    void SceneObjectManager::postRender() {
        destroyObjectsInQueue();
    }

    SceneObjectHandleBase SceneObjectManager::registerObject(const std::shared_ptr<SceneObject>& _object) {
        auto id = ++mNextId;
        _object->initialize(_object, id);
        auto handle = SceneObjectHandleBase(_object);
        mObjects[id] = handle;
        return handle;
    }

    void SceneObjectManager::unregisterObject(SceneObjectHandleBase& _object) {
        mObjects.erase(_object.getInstanceId());
        onObjectDestroyed.trigger(static_scene_object_cast<GameObject>(_object));
        _object.destroy();
    }

    SceneObjectHandleBase SceneObjectManager::getObject(const uint64_t& _instanceId) const {
        auto it = mObjects.find(_instanceId);
        if (it != mObjects.end())
            return it->second;
        return nullptr;
    }

    bool SceneObjectManager::objectExists(const uint64_t& _instanceId) const {
        return mObjects.find(_instanceId) != mObjects.end();
    }

    void SceneObjectManager::pushToDestroyQueue(const SceneObjectHandleBase& _object) {
        if (_object.isDestroyed())
            return;
        auto id = _object.getInstanceId();
        mDestroyQueue[id] = _object;
        _object->_setIsInDestroyQueue();
    }

    void SceneObjectManager::destroyObjectsInQueue() {
        for (auto& objPair : mDestroyQueue)
            objPair.second.mData->mPtr->object->destroyInternal(objPair.second, true);
        mDestroyQueue.clear();
    }

}

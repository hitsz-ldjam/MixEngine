#include "MxSceneObjectHandle.h"
#include "../Exceptions/MxExceptions.hpp"

namespace Mix {
    MX_IMPLEMENT_RTTI(SceneObjectHandleBase, Object);

    bool SceneObjectHandleBase::isDestroyed() const {
        return mData->mPtr == nullptr || mData->mPtr->object == nullptr || mData->mPtr->object->_isDestroyed();
    }

    std::shared_ptr<SceneObject> SceneObjectHandleBase::get() const {
        throwIfDestroyed();

        return mData->mPtr->object;
    }

    SceneObjectHandleBase::SceneObjectHandleBase(const std::shared_ptr<SceneObject>& _ptr) {
        mData = std::make_shared<SceneObjectHandleData>(_ptr->mInstanceData);
    }

    void SceneObjectHandleBase::throwIfDestroyed() const {
        if (isDestroyed())
            MX_EXCEPT("SceneObject has been destroyed");
    }

    void SceneObjectHandleBase::destroy() {
        if (mData->mPtr != nullptr)
            mData->mPtr->object = nullptr;
    }

    SceneObject* SceneObjectHandleBase::getRawPtr() const {
        throwIfDestroyed();

        return mData->mPtr->object.get();
    }
}

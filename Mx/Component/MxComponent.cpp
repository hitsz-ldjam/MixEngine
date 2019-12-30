#include "MxComponent.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(Component, Object);

    void Component::destroy(bool _immediate) {
        mGameObject->removeComponent(mThisHandle, _immediate);
    }

    HTransform Component::transform() const {
        return mGameObject->transformHandle();
    }

    void Component::destroyInternal(SceneObjectHandleBase& _handle, bool _immediate) {
        if (_immediate)
            SceneObjectManager::Get()->unregisterObject(_handle);
        else
            SceneObjectManager::Get()->pushToDestroyQueue(_handle);
    }

    Component::Component(const HGameObject& _gameObject) :mGameObject(_gameObject) {
    }
}

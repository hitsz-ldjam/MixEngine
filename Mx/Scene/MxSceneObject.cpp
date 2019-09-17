#include "MxSceneObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(SceneObject, Object);

    void SceneObject::initialize(std::shared_ptr<SceneObject> _object, uint64_t _instanceId) {
        mInstanceData = std::make_shared<SceneObjectInstanceData>();
        mInstanceData->object = std::move(_object);
        mInstanceData->instanceId = _instanceId;
    }
}

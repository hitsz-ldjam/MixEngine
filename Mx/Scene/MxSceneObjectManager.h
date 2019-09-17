#pragma once
#ifndef MX_SCENE_OBJECT_MANAGER_H_
#define MX_SCENE_OBJECT_MANAGER_H_
#include "../Engine/MxModuleBase.h"
#include "../Utils/MxEvent.h"
#include "../Definitions/MxDefinitions.h"
#include <map>

namespace Mix {
    class SceneObject;
    class SceneObjectHandleBase;

    class SceneObjectManager :public ModuleBase {
    public:
        static SceneObjectManager* Get();

        SceneObjectManager() = default;

        ~SceneObjectManager() = default;

        void load() override {}

        void init() override {}

        void update();

        void lateUpdate();

        void postRender();

        SceneObjectHandleBase registerObject(const std::shared_ptr<SceneObject>& _object);

        void unregisterObject(SceneObjectHandleBase& _object);

        SceneObjectHandleBase getObject(const uint64_t& _instanceId) const;

        bool objectExists(const uint64_t& _instanceId) const;

        void pushToDestroyQueue(const SceneObjectHandleBase& _object);

        void destroyObjectsInQueue();

        Event<void(HGameObject)> onObjectDestroyed;

    private:
        uint64_t mNextId = 0;
        std::map<uint64_t, SceneObjectHandleBase> mObjects;
        std::map<uint64_t, SceneObjectHandleBase> mDestroyQueue;
    };
}

#endif

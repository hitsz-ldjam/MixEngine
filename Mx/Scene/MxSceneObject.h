#pragma once
#ifndef MX_SCENE_OBJECT_H_
#define MX_SCENE_OBJECT_H_
#include "../Object/MxObject.h"
#include "../Utils/MxGuid.h"

namespace Mix {
    class SceneObjectManager;
    class SceneObjectHandleBase;
    class SceneObject;

    template<typename _Ty>
    class SceneObjectHandle;

    /**
     * \brief Internal data of an SceneObject instance which will be shared by all SceneObject handles that point to the object.
     */
    struct SceneObjectInstanceData {
        SceneObjectInstanceData() :object(nullptr), instanceId(0) {}

        std::shared_ptr<SceneObject> object;
        uint64_t instanceId;
    };


    /**
     * \brief The base of all objects that can be registered into a Scene.
     */
    class SceneObject :public Object {
        MX_DECLARE_RTTI;

        friend class SceneObjectManager;
        friend class SceneObjectHandleBase;
    public:
        SceneObject() = default;

        virtual ~SceneObject() = default;

        const std::string& getName() const { return mName; }

        void setName(const std::string& _name) { mName = _name; }

        const UUID& getUUID() const { return mUUID; }

        const uint64_t& getInstanceId() const { return mInstanceData->instanceId; }

    public:
        void _setUUID(const UUID& _uuid) { mUUID = _uuid; }

        void _setInstanceId(const uint64_t& _instanceId) { mInstanceData->instanceId = _instanceId; }

        void _setIsDestroyed() { mIsDestroyed = true; }

        bool _isDestroyed() const { return mIsDestroyed; }

        std::shared_ptr<SceneObjectInstanceData> _getInstanceData() const { return mInstanceData; }

        void initialize(std::shared_ptr<SceneObject> _object, uint64_t _instanceId);

        virtual void destroyInternal(SceneObjectHandleBase& _handle, bool _immediate = false) = 0;

    protected:
        std::string mName;
        UUID mUUID;

    private:
        bool mIsDestroyed = false;
        std::shared_ptr<SceneObjectInstanceData> mInstanceData;
    };



}

#endif

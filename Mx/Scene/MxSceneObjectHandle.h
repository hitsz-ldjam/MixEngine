#pragma once
#ifndef MX_SCENE_OBJECT_HANDLE_H_
#define MX_SCENE_OBJECT_HANDLE_H_

#include "../Object/MxObject.h"
#include "MxSceneObject.h"
#include <memory>

namespace Mix {
     /**
     * \brief Internal data of SceneObjectHandle whick can be shared between SceneObject handles;
     */
    struct SceneObjectHandleData {
        SceneObjectHandleData() = default;

        SceneObjectHandleData(std::shared_ptr<SceneObjectInstanceData> _ptr) :mPtr(std::move(_ptr)) {}

        std::shared_ptr<SceneObjectInstanceData> mPtr;
    };


    /**
     * \brief The base class of handles that refer to various types of scene objects used to track the whether the object is still alive
     */
    class SceneObjectHandleBase : public Object {
        MX_DECLARE_RTTI;

        friend class SceneObjectManager;
    public:
        SceneObjectHandleBase() :mData(nullptr) {}

        bool isDestroyed() const;

        uint64_t getInstanceId() const { return mData->mPtr ? mData->mPtr->instanceId : 0; }

        std::shared_ptr<SceneObject> get() const;

        SceneObject* operator->() const { return getRawPtr(); }

        SceneObject& operator*() const { return *getRawPtr(); }

        std::shared_ptr<SceneObjectHandleData> _getHandleData() const { return mData; }

    protected:
        SceneObjectHandleBase(const std::shared_ptr<SceneObject>& _ptr);

        SceneObjectHandleBase(std::shared_ptr<SceneObjectHandleData> _data) :mData(std::move(_data)) {};

        SceneObjectHandleBase(std::nullptr_t) :mData(nullptr) {}

        bool isDestroyedInternal(bool _checkQueue) const;

        void throwIfDestroyed() const;

        void destroy();

        SceneObject* getRawPtr() const;

        std::shared_ptr<SceneObjectHandleData> mData;

    };



    template<typename _Ty>
    class SceneObjectHandle : public SceneObjectHandleBase {
    public:
        SceneObjectHandle() :SceneObjectHandleBase() {
            mData = std::make_shared<SceneObjectHandleData>();
        }

        SceneObjectHandle(const SceneObjectHandle& _other) = default;

        SceneObjectHandle(SceneObjectHandle&& _other) = default;

        SceneObjectHandle(std::nullptr_t) :SceneObjectHandle() {
        }

        SceneObjectHandle& operator=(std::nullptr_t) {
            mData = std::make_shared<SceneObjectHandleData>();
            return *this;
        }

        SceneObjectHandle& operator=(const SceneObjectHandle& _other) = default;

        SceneObjectHandle& operator=(SceneObjectHandle&& _other) = default;

        std::shared_ptr<_Ty> get() const {
            return std::static_pointer_cast<_Ty>(SceneObjectHandleBase::get());
        }

        _Ty* operator->() const { return getRawPtr(); }

        _Ty& operator*() const { return *getRawPtr(); }

        explicit operator bool() const {
            return mData->mPtr != nullptr && mData->mPtr->object != nullptr;
        }

        template<typename _T>
        bool operator==(const SceneObjectHandle<_T>& _other) const {
            return (mData == nullptr && _other.mData == nullptr) ||
                (mData != nullptr && _other.mData != nullptr && getInstanceId() == _other.getInstanceId());
        }

        template<typename _T>
        bool operator!=(const SceneObjectHandle<_T>& _other) const {
            return !(*this == _other);
        }

        bool operator==(std::nullptr_t) const {
            return mData->mPtr == nullptr || mData->mPtr->object == nullptr;
        }

        bool operator!=(std::nullptr_t) const {
            return !(*this == nullptr);
        }

        template<class _Ty1, class _Ty2>
        friend SceneObjectHandle<_Ty1> static_scene_object_cast(const SceneObjectHandle<_Ty2>& other);

        template<class _Ty1>
        friend SceneObjectHandle<_Ty1> static_scene_object_cast(const SceneObjectHandleBase& other);

    protected:
        _Ty* getRawPtr() const {
            return reinterpret_cast<_Ty*>(SceneObjectHandleBase::getRawPtr());
        }

        SceneObjectHandle(std::shared_ptr<SceneObjectHandleData> _data) :SceneObjectHandleBase(std::move(_data)) {}
    };


    template<class _Ty1, class _Ty2>
    SceneObjectHandle<_Ty1> static_scene_object_cast(const SceneObjectHandle<_Ty2>& _other) {
        return SceneObjectHandle<_Ty1>(_other.mData);
    }

    template<class _Ty1>
    SceneObjectHandle<_Ty1> static_scene_object_cast(const SceneObjectHandleBase& _other) {
        return SceneObjectHandle<_Ty1>(_other._getHandleData());
    }
}

#endif

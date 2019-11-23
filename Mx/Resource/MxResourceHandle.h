#pragma once
#ifndef MX_RESOURCE_HANDLE_H_
#define MX_RESOURCE_HANDLE_H_
#include "../Utils/MxGuid.h"
#include <memory>
#include <atomic>
#include <condition_variable>


namespace Mix {
    class Resource;

    struct ResourceHandleData {
        std::shared_ptr<Resource> ptr;
        UUID uuid;
        bool isCreated = false; // Indicates that the resource has been created, but may not loaded.
        std::atomic<uint32> refCount = 0;
    };


    class ResourceHandleBase {
    public:
        bool isLoaded(bool _checkDependencies) const;

        void waitUtilLoaded() const;

        void release();

        const UUID& getUUID() const;

    protected:
        void destroy();

        void setHandleData(const std::shared_ptr<Resource>& _resource, const UUID& _uuid);

        std::shared_ptr<ResourceHandleData> getHandleData() const { return mData; }

        void clearHandleData();

        void incrInternalRef();

        void decrInternalRef();

        void onLoadComplete();

        void throwIfNotLoaded() const;

        std::shared_ptr<ResourceHandleData> mData;

    private:
        std::condition_variable mLoadCompleteCondition;
        static std::mutex sResourceCreatedMutex;

    };



    template<bool _IsWeakHandle>
    class TResourceHandleBase :public ResourceHandleBase {};


    /////////////////////////////////////////////////////////////
    //                  Weak handle base                       //
    /////////////////////////////////////////////////////////////

    template<>
    class TResourceHandleBase<true> :public ResourceHandleBase {
    protected:
        void incrRef() { /* Do nothing */ }

        void decrRef() { /* Do nothing */ }
    };


    /////////////////////////////////////////////////////////////
    //                  Strong handle base                     //
    /////////////////////////////////////////////////////////////

    template<>
    class TResourceHandleBase<false> :public ResourceHandleBase {
    protected:
        void incrRef() {
            if (mData)
                mData->refCount.fetch_add(1, std::memory_order_relaxed);
        }

        void decrRef() {
            if (mData) {
                uint32 refCount = mData->refCount.fetch_sub(1, std::memory_order_release);

                if (refCount == 1) {
                    std::atomic_thread_fence(std::memory_order_acquire);
                    destroy();
                }
            }
        }
    };


    /////////////////////////////////////////////////////////////
    //          Weak/Strong handle for different type          //
    /////////////////////////////////////////////////////////////

    template<typename _Ty, bool _IsWeakHandle>
    class TResourceHandle :public TResourceHandleBase<_IsWeakHandle> {
    public:
        using ValueType = _Ty;
        using ReferenceType = _Ty & ;
        using PointerType = _Ty * ;

        using StrongHandleType = TResourceHandle<_Ty, false>;
        using WeakHandleType = TResourceHandle<_Ty, true>;

        TResourceHandle() = default;

        TResourceHandle(std::nullptr_t) {}

        TResourceHandle(const TResourceHandle& _other) {
            this->mData = _other.mData;
            this->incrRef();
        }

        TResourceHandle(TResourceHandle&& _other) = default;

        TResourceHandle<_Ty, _IsWeakHandle>& operator=(const TResourceHandle<_Ty, _IsWeakHandle>& _other) {
            setHandleData(_other.getHandleData());
            return *this;
        }

        TResourceHandle<_Ty, _IsWeakHandle>& operator=(TResourceHandle<_Ty, _IsWeakHandle>&& _other) {
            if (this == &_other)
                return *this;

            this->decrRef();
            this->mData = std::exchange(_other.mData, nullptr);

            return *this;
        }

        ~TResourceHandle() {
            this->decrRef();
        }

        _Ty* getRawPtr() const {

            return this->mData == nullptr ? nullptr : static_cast<_Ty*>(this->mData->ptr.get());
        }

        std::shared_ptr<_Ty> get() const {
            return this->mData == nullptr ? nullptr : std::static_pointer_cast<_Ty>(this->mData->ptr);
        }

        _Ty* operator->() const { this->throwIfNotLoaded(); return getRawPtr(); }

        _Ty& operator*() const { this->throwIfNotLoaded(); return *getRawPtr(); }

        explicit operator bool() const {
            return this->mData != nullptr && !(this->mData->uuid.empty());
        }

        bool operator==(std::nullptr_t) const {
            return this->mData == nullptr || this->mData->uuid.empty();
        }

        TResourceHandle<_Ty, _IsWeakHandle>& operator=(std::nullptr_t) {
            this->decrRef();
            this->mData = nullptr;
            return *this;
        }

        operator TResourceHandle<Resource, _IsWeakHandle>() const {
            TResourceHandle<Resource, _IsWeakHandle> handle;
            handle.setHandleData(this->getHandleData());
            return handle;
        }

        std::enable_if_t<!_IsWeakHandle, WeakHandleType> getWeak() const {
            WeakHandleType handle;
            handle.setHandleData(this->getHandleData());
            return handle;
        }

        std::enable_if_t<_IsWeakHandle, StrongHandleType> lock() const {
            StrongHandleType handle;
            handle.setHandleData(this->getHandleData());
            return handle;
        }


    protected:
        explicit TResourceHandle(_Ty* _ptr, const UUID& _uuid) {
            this->mData = std::make_shared<ResourceHandleData>();
            this->incrRef();

            this->setHandleData(std::shared_ptr<_Ty>(_ptr), _uuid);
            this->mData->isCreated = true;
        }

        void setHandleData(const std::shared_ptr<ResourceHandleData>& _data) {
            this->decrRef();
            this->mData = _data;
            this->incrRef();
        }

        using ResourceHandleBase::setHandleData;


    /////////////////////////////////////////////////////////////
    //                    friend functions                     //
    /////////////////////////////////////////////////////////////

    public:
        template<typename _Ty1, typename _Ty2, bool _IsWeak2, bool _IsWeak1 = false>
        friend TResourceHandle<_Ty1, _IsWeak1>  static_resource_handle_cast(const TResourceHandle<_Ty2, _IsWeak2>& _other);


    };

    template<typename _Ty1, typename _Ty2, bool _IsWeak2, bool _IsWeak1>
    TResourceHandle<_Ty1, _IsWeak1>  static_resource_handle_cast(const TResourceHandle<_Ty2, _IsWeak2>& _other) {
        TResourceHandle<_Ty1, _IsWeak1> handle;
        handle.setHandleData(_other.getHandleData());

        return handle;
    }

    template<typename _Ty, bool _IsWeak>
    bool operator==(std::nullptr_t, const TResourceHandle<_Ty, _IsWeak>& _handle) {
        return _handle == nullptr;
    }

}

#endif

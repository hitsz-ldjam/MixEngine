#pragma once
#ifndef MX_REFERENCE_MGR_H_
#define MX_REFERENCE_MGR_H_

#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace Mix {
    namespace Utils {
        template<typename _Value>
        struct Referenced {
            size_t count;
            _Value value;

            Referenced() :count(0), value() {}

            Referenced(const size_t _count, const _Value& _value) :count(_count), value(_value) {}

            Referenced(const size_t _count, _Value&& _value) :count(_count), value(std::move(_value)) {}

            template<typename... _Args>
            explicit Referenced(const size_t _count, _Args&& ... _args) : count(_count), value(std::forward<_Args>(_args)...) {}
        };

        template<typename _RefMgr>
        class Reference;

        template<typename _Key, typename _Value>
        class ReferenceMgr {
        public:
            using KeyType = _Key;
            using ValueType = _Value;
            using ReferencedType = Referenced<ValueType>;
            using ReferenceType = Reference<ReferenceMgr>;
            friend ReferenceType;

            ReferenceMgr() = default;

            ~ReferenceMgr() = default;

            ReferenceMgr(const ReferenceMgr& _other) = delete;

            ReferenceMgr(ReferenceMgr&& _other) noexcept = default;

            ReferenceMgr& operator=(const ReferenceMgr& _other) = delete;

            ReferenceMgr& operator=(ReferenceMgr&& _other) noexcept = default;

            ReferenceType AddElement(const _Key& _key, const ValueType& _valuePtr) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, _valuePtr));
                return ReferenceType(this, _key);
            }

            ReferenceType AddElement(const _Key& _key, ValueType&& _valuePtr) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, std::move(_valuePtr)));
                return ReferenceType(this, _key);
            }

            template<typename... _Args>
            ReferenceType AddElement(const _Key& _key, _Args&& ..._args) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, std::forward<_Args>(_args)...));
                return ReferenceType(this, _key);
            }

            bool HasKey(const _Key& _key) const {
                return mMap.count(_key);
            }

            size_t RefCount(const _Key& _key) {
                if (mMap.count(_key))
                    return mMap[_key].count;
                return 0;
            }

            ReferenceType GetRef(const _Key& _key) {
                if (mMap.count(_key)) {
                    ++mMap[_key].count;
                    return ReferenceType(this, _key);
                }
                return nullptr;
            }

            ValueType* GetRawPtr(const _Key& _key) {
                if (mMap.count(_key))
                    return &mMap[_key].value;
                return nullptr;
            }

            void Clear() {
                mMap.clear();
            }

        private:
            std::unordered_map<_Key, ReferencedType> mMap;

            void AddRef(const _Key& _key) {
                if (mMap.count(_key))
                    ++(mMap[_key].count);
            }

            void DecrRef(const _Key& _key) {
                if (mMap.count(_key)) {
                    if (mMap[_key].count == 1)
                        mMap.erase(_key);
                    else
                        --mMap[_key].count;
                }
            }

        };

        class NullRef :public std::exception {
        public:
            NullRef() :exception("Try to dereference a NULL Reference") {}
        };

        template<typename _RefMgr>
        class Reference {
        public:
            using KeyType = typename _RefMgr::KeyType;
            using ValueType = typename _RefMgr::ValueType;

            Reference(_RefMgr* _mgr, const KeyType _key) :mMgr(_mgr), mKey(_key) {}
            Reference(const std::nullptr_t&) :mMgr(nullptr), mKey() {}

            Reference(const Reference& _other) {
                *this = _other;
            }

            Reference(Reference&& _other) noexcept {
                *this = std::move(_other);
            }

            Reference& operator=(const Reference& _other) {
                if (mMgr)
                    mMgr->DecrRef(mKey);

                mMgr = _other.mMgr;
                mKey = _other.mKey;
                if (mMgr)
                    mMgr->AddRef(mKey);
                return *this;
            }

            Reference& operator=(Reference&& _other) noexcept {
                return this->Swap(_other);
            }

            Reference& operator=(const std::nullptr_t&) {
                return *this = Reference(nullptr);
            }

            explicit operator bool()const {
                return mMgr != nullptr;
            }

            ~Reference() {
                if (mMgr)
                    mMgr->DecrRef(mKey);
            }

            size_t RefCount() const {
                if (mMgr)
                    return mMgr->RefCount(mKey);
                return 0;
            }

            ValueType& operator*() {
                if (mMgr)
                    return *(mMgr->GetRawPtr(mKey));
                throw NullRef();
            }

            ValueType* operator->() {
                if (mMgr)
                    return mMgr->GetRawPtr(mKey);
                throw NullRef();
            }

            Reference& Swap(Reference& _other) {
                std::swap(mMgr, _other.mMgr);
                std::swap(mKey, _other.mKey);
                return *this;
            }

        private:
            _RefMgr* mMgr;
            KeyType mKey;
        };

    }
}
#endif

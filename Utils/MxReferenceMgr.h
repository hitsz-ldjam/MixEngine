#pragma once
#ifndef MX_REFERENCE_MGR_H_
#define MX_REFERENCE_MGR_H_

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <functional>

namespace Mix {
    namespace Utils {
        template<typename _Value>
        struct Referenced {
            size_t count;
            _Value value;

            Referenced() :count(0), value() {}

            Referenced(const size_t _count, const _Value& _value) :
                count(_count),
                value(_value) {
            }

            Referenced(const size_t _count, _Value&& _value) :
                count(_count),
                value(std::move(_value)) {
            }

            template<typename... _Args>
            explicit Referenced(const size_t _count, _Args&& ... _args) :
                count(_count),
                value(std::forward<_Args>(_args)) {
            }

            Referenced(const Referenced& _other) = delete;

            Referenced(Referenced&& _other) noexcept {
                count = _other.count;
                value = _other.value;
                _other.value = nullptr;
            }
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
            using RemoveOpType = std::function<void(ValueType&)>;

            friend ReferenceType;

            ReferenceMgr() = default;

            explicit ReferenceMgr(const RemoveOpType& _op) :mRemoveOp(_op) {}

            explicit ReferenceMgr(RemoveOpType&& _op) :mRemoveOp(std::move(_op)) {}

            ~ReferenceMgr() {
                clear();
            };

            ReferenceMgr(const ReferenceMgr& _other) = delete;

            ReferenceMgr(ReferenceMgr&& _other) noexcept = default;

            ReferenceMgr& operator=(const ReferenceMgr& _other) = delete;

            ReferenceMgr& operator=(ReferenceMgr&& _other) noexcept = default;

            void setRemoveOp(const RemoveOpType& _op) {
                mRemoveOp = _op;
            }

            void setRemoveOp(RemoveOpType&& _op) {
                mRemoveOp = std::move(_op);
            }

            ReferenceType insert(const _Key& _key, const ValueType& _value) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, _value));
                return ReferenceType(this, _key);
            }

            ReferenceType insert(const _Key& _key, ValueType&& _value) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, std::move(_value)));
                return ReferenceType(this, _key);
            }

            template<typename... _Args>
            ReferenceType emplace(const _Key& _key, _Args&& ..._args) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, std::forward<_Args>(_args)...));
                return ReferenceType(this, _key);
            }

            bool hasKey(const _Key& _key) const {
                return mMap.count(_key);
            }

            size_t refCount(const _Key& _key) {
                typename std::unordered_map<_Key, ReferencedType>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    return it->second.count;
                return 0;
            }

            ReferenceType getReference(const _Key& _key) {
                typename std::unordered_map<_Key, ReferencedType>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end()) {
                    ++it->second.count;
                    return ReferenceType(this, _key);
                }
                return nullptr;
            }

            ValueType* getPtr(const _Key& _key) {
                typename std::unordered_map<_Key, ReferencedType>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    return &(it->second.value);
                return nullptr;
            }

            void clear() {
                for (auto it = mMap.begin(); it != mMap.end();)
                    remove(it++);
            }

        private:
            using MapType = std::unordered_map<_Key, ReferencedType>;

            MapType mMap;
            std::function<void(ValueType&)> mRemoveOp = nullptr;

            void addRef(const _Key& _key) {
                typename MapType::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    ++it->second.count;
            }

            void decrRef(const _Key& _key) {
                typename MapType::iterator it;
                if ((it = mMap.find(_key)) != mMap.end()) {
                    if (it->second.count == 1)
                        remove(it);
                    else
                        --it->second.count;
                }
            }

            void remove(const typename MapType::iterator& _it) {
                if (mRemoveOp)
                    mRemoveOp(_it->second.value);
                mMap.erase(_it);
            }
        };

        class NullRef final :public std::exception {
        public:
            NullRef() :exception("Try to dereference a NULL Reference") {}
        };

        template<typename _RefMgr>
        class Reference {
            friend _RefMgr;
        public:
            using KeyType = typename _RefMgr::KeyType;
            using ValueType = typename _RefMgr::ValueType;

            Reference(const std::nullptr_t&) :mMgr(nullptr), mKey() {}

            Reference(const Reference& _other) {
                if (mMgr)
                    mMgr->decrRef(mKey);

                mMgr = _other.mMgr;
                mKey = _other.mKey;

                if (mMgr)
                    mMgr->addRef(mKey);
            }

            Reference(Reference&& _other) noexcept {
                mMgr = _other.mMgr;
                mKey = _other.mKey;
                _other.mMgr = nullptr;
            }

            Reference& operator=(const Reference& _other) {
                if (mMgr)
                    mMgr->decrRef(mKey);

                mMgr = _other.mMgr;
                mKey = _other.mKey;

                if (mMgr)
                    mMgr->addRef(mKey);

                return *this;
            }

            Reference& operator=(Reference&& _other) noexcept {
                return this->swap(_other);
            }

            Reference& operator=(const std::nullptr_t&) {
                return *this = Reference(nullptr);
            }

            explicit operator bool()const {
                return mMgr != nullptr;
            }

            ~Reference() {
                if (mMgr)
                    mMgr->decrRef(mKey);
            }

            size_t refCount() const {
                if (*this)
                    return mMgr->refCount(mKey);
                return 0;
            }

            ValueType& operator*() const {
                if (*this)
                    return *mMgr->getPtr(mKey);
                throw NullRef();
            }

            ValueType* operator->() const {
                if (*this)
                    return mMgr->getPtr(mKey);
                throw NullRef();
            }

            Reference& swap(Reference& _other) noexcept {
                std::swap(mMgr, _other.mMgr);
                std::swap(mKey, _other.mKey);
                return *this;
            }

            ValueType* ptr() const {
                if (*this)
                    return mMgr->getPtr(mKey);
                return nullptr;
            }

            template<typename _Ty>
            _Ty& to() const {
                return static_cast<_Ty&>(*mMgr->getPtr(mKey));
            }

            template<typename _Ty>
            _Ty dynamicCast() const {
                return dynamic_cast<_Ty>(*mMgr->getPtr(mKey));
            }

        private:
            Reference(_RefMgr* _mgr, const KeyType& _key) :
                mMgr(_mgr), mKey(_key) {
            }

            _RefMgr* mMgr = nullptr;
            KeyType mKey;

        };

    }
}
#endif

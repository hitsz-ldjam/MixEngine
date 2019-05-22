#pragma once
#ifndef MX_RESOURCE_REF_MGR_H_
#define MX_RESOURCE_REF_MGR_H_

#include "../Utils/MxReferenceMgr.h"
#include "../Definitions/MxDefinitions.h"

#include <memory>

namespace Mix {
    namespace Resource {
        class ResourceBase;
        
        class ResourceRef;

        class ResourceRefMgr {
            struct Node {
                using PtrType = ResourceBase * ;
                size_t count;
                PtrType value;

                Node() :count(0), value() {}

                Node(const size_t _count, PtrType const & _value) :
                    count(_count),
                    value(_value) {
                }

                Node(const size_t _count, PtrType&& _value) :
                    count(_count),
                    value(std::move(_value)) {
                }

                Node(const Node& _other) = delete;

                Node(Node&& _other) noexcept {
                    count = _other.count;
                    value = _other.value;
                    _other.value = nullptr;
                }
            };

            friend ResourceRef;

        public:
            using KeyType = Guid;
            using ValueType = ResourceBase * ;
            using ReferenceType = ResourceRef;

            ResourceRefMgr() = default;

            ~ResourceRefMgr() = default;

            ResourceRefMgr(const ResourceRefMgr& _other) = delete;

            ResourceRefMgr(ResourceRefMgr&& _other) noexcept = default;

            ResourceRefMgr& operator=(const ResourceRefMgr& _other) = delete;

            ResourceRefMgr& operator=(ResourceRefMgr&& _other) noexcept = default;

            ReferenceType insert(const KeyType& _key, ValueType const& _value) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, _value));
                return ReferenceType(this, _key);
            }

            ReferenceType insert(const KeyType& _key, ValueType&& _value) {
                mMap.emplace(std::piecewise_construct,
                             std::forward_as_tuple(_key),
                             std::forward_as_tuple(1, std::move(_value)));
                return ReferenceType(this, _key);
            }

            bool hasKey(const KeyType& _key) const {
                return mMap.count(_key);
            }

            size_t refCount(const KeyType& _key) {
                typename std::unordered_map<KeyType, Node>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    return it->second.count;
                return 0;
            }

            ReferenceType getReference(const KeyType& _key) {
                typename std::unordered_map<KeyType, Node>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end()) {
                    ++it->second.count;
                    return ReferenceType(this, _key);
                }
                return nullptr;
            }

            ValueType* getPtr(const KeyType& _key) {
                typename std::unordered_map<KeyType, Node>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    return &(it->second.value);
                return nullptr;
            }

            void clear() {
                mMap.clear();
            }

        private:
            std::unordered_map<KeyType, Node> mMap;

            void addRef(const KeyType& _key) {
                std::unordered_map<KeyType, Node>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end())
                    ++it->second.count;
            }

            void decrRef(const KeyType& _key) {
                std::unordered_map<KeyType, Node>::iterator it;
                if ((it = mMap.find(_key)) != mMap.end()) {
                    if (it->second.count == 1)
                        mMap.erase(it);
                    else
                        --it->second.count;
                }
            }

        };

        class NullRef final :public std::exception {
        public:
            NullRef() :exception("Try to dereference a NULL Reference") {}
        };

        class ResourceRef {
            friend ResourceRefMgr;
        public:
            using KeyType = ResourceRefMgr::KeyType;
            using ValueType = ResourceRefMgr::ValueType;

            ResourceRef(const std::nullptr_t&) :mMgr(nullptr), mKey() {}

            ResourceRef(const ResourceRef& _other) {
                *this = _other;
            }

            ResourceRef(ResourceRef&& _other) noexcept {
                *this = std::move(_other);
            }

            ResourceRef& operator=(const ResourceRef& _other) {
                if (mMgr)
                    mMgr->decrRef(mKey);

                mMgr = _other.mMgr;
                mKey = _other.mKey;

                if (mMgr)
                    mMgr->addRef(mKey);

                return *this;
            }

            ResourceRef& operator=(ResourceRef&& _other) noexcept {
                return this->swap(_other);
            }

            ResourceRef& operator=(const std::nullptr_t&) {
                return *this = ResourceRef(nullptr);
            }

            explicit operator bool()const {
                return mMgr != nullptr;
            }

            ~ResourceRef() {
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

            ResourceRef& swap(ResourceRef& _other) noexcept {
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
            _Ty& to() {
                return static_cast<_Ty&>(*mMgr->getPtr(mKey));
            }


        private:
            ResourceRef(ResourceRefMgr* _mgr, const KeyType& _key) :
                mMgr(_mgr), mKey(_key) {
            }

            ResourceRefMgr* mMgr;
            KeyType mKey;
        };
    }
}

#endif

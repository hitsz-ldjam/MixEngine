#pragma once
#ifndef MX_LITE_SHARED_PTR_H_
#define MX_LITE_SHARED_PTR_H_

#include <cstdint>
#include <type_traits>

namespace Mix {

   /**
    * \brief A lite smart pointer.
    *
    * \note NOT support multi-threads or array.
    */
    template<typename _Ty>
    class LiteSharedPtr {
    public:
        static_assert(!std::is_array_v<_Ty>, "LiteSharedPtr CAN NOT be used to hold array, use LiteSharedPtr<std::array<T>> or something similar instead.");

        using element_type = _Ty;

        ~LiteSharedPtr() {
            reset();
        }

        LiteSharedPtr() noexcept :mPtr(nullptr), mCounter(nullptr) {}

        LiteSharedPtr(std::nullptr_t) noexcept :LiteSharedPtr() {}

        explicit LiteSharedPtr(_Ty* _ptr) {
            reset(_ptr);
        }

        LiteSharedPtr(const LiteSharedPtr& _other) {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;
            incrRef();
        }

        template<class _U, std::enable_if_t<std::is_convertible_v<_U*, _Ty*>, int> = 0 >
        LiteSharedPtr(const LiteSharedPtr<_U>& _other) {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;
            incrRef();
        }

        LiteSharedPtr(LiteSharedPtr&& _other) noexcept {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;

            _other.mPtr = nullptr;
            _other.mCounter = nullptr;
        }

        template<class _U, std::enable_if_t<std::is_convertible_v<_U*, _Ty*>, int> = 0 >
        LiteSharedPtr(LiteSharedPtr<_U>&& _other) noexcept {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;

            _other.mPtr = nullptr;
            _other.mCounter = nullptr;
        }

        template<typename _U>
        LiteSharedPtr(const LiteSharedPtr<_U>& _lsp, element_type* _ptr) {
            mPtr = _ptr;
            mCounter = _lsp.mCounter;
        }

        LiteSharedPtr& operator=(const LiteSharedPtr& _other) {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;
            incrRef();

            return *this;
        }

        template<class _U, std::enable_if_t<std::is_convertible_v<_U*, _Ty*>, int> = 0 >
        LiteSharedPtr& operator=(const LiteSharedPtr<_U>& _other) {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;
            incrRef();

            return *this;
        }

        LiteSharedPtr& operator=(LiteSharedPtr&& _other) noexcept {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;

            _other.mPtr = nullptr;
            _other.mCounter = nullptr;
            return *this;
        }

        template<class _U, std::enable_if_t<std::is_convertible_v<_U*, _Ty*>, int> = 0 >
        LiteSharedPtr& operator=(LiteSharedPtr<_U>&& _other) noexcept {
            mPtr = _other.mPtr;
            mCounter = _other.mCounter;

            _other.mPtr = nullptr;
            _other.mCounter = nullptr;
            return *this;
        }

        void reset() noexcept {
            decrRef();
            mPtr = nullptr;
            mCounter = nullptr;
        }

        template<class _U, std::enable_if_t<std::is_convertible_v<_U*, _Ty*>, int> = 0 >
        void reset(_U* _ptr) noexcept {
            decrRef();
            mPtr = _ptr;
            incrRef();
        }

        void swap(LiteSharedPtr& _other) noexcept {
            std::swap(mPtr, _other.mPtr);
            std::swap(mCounter, _other.mCounter);
        }

        element_type* get() const noexcept {
            return mPtr;
        }

        _Ty& operator*() const noexcept {
            return *mPtr;
        }

        _Ty* operator->() const noexcept {
            return mPtr;
        }

        element_type& operator[](std::ptrdiff_t _idx) const {
            return mPtr[_idx];
        }

        uint32_t use_count() const noexcept {
            return mCounter == nullptr ? 0 : *mCounter;
        }

        explicit operator bool() const noexcept {
            return get();
        }

    private:
        void incrRef() {
            if (!mCounter) {
                mCounter = new uint32_t(0);
            }

            ++(*mCounter);
        }

        void decrRef() {
            if (mCounter) {
                if (*mCounter == 1) {
                    delete mPtr;
                }
                else
                    --(*mCounter);
            }
        }

        _Ty* mPtr;
        uint32_t* mCounter;
    };


    template<typename _Ty>
    using LSPtr = LiteSharedPtr<_Ty>;


    template<typename _Ty, typename ... _Args>
    LiteSharedPtr<_Ty> make_lite_shared(_Args&&... _args) {
        return LiteSharedPtr<_Ty>(new _Ty(std::forward<_Args>(_args)...));
    }

    template<typename _T, typename _U>
    LiteSharedPtr<_T> static_pointer_cast(const LiteSharedPtr<_U>& _other) noexcept {
        auto p = static_cast<typename LiteSharedPtr<_T>::element_type*>(_other.get());
        return LiteSharedPtr<_T>(_other, p);
    }

    template<typename _T, typename _U>
    LiteSharedPtr<_T> dynamic_pointer_cast(const LiteSharedPtr<_U>& _other) noexcept {
        if (auto p = dynamic_cast<typename LiteSharedPtr<_T>::element_type*>(_other.get()))
            return LiteSharedPtr<_T>(_other, p);
        return LiteSharedPtr<_T>();
    }

    template<typename _T, typename _U>
    LiteSharedPtr<_T> const_pointer_cast(const LiteSharedPtr<_T>& _other) noexcept {
        auto p = const_cast<typename LiteSharedPtr<_T>::element_type*>(_other.get());
        return LiteSharedPtr<_T>(_other, p);
    }

    template<typename _T, typename _U>
    LiteSharedPtr<_T> reinterpret_pointer_cast(const LiteSharedPtr<_T>& _other) noexcept {
        auto p = reinterpret_cast<typename LiteSharedPtr<_T>::element_type*>(_other.get());
        return LiteSharedPtr<_T>(_other, p);
    }

    template<typename _T, typename _U>
    bool operator==(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return _lhs.get() == _rhs.get();
    }

    template<typename _T, typename _U>
    bool operator!=(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return !(_lhs == _rhs);
    }

    template<typename _T, typename _U>
    bool operator<(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return _lhs.get() < _rhs.get();
    }

    template<typename _T, typename _U>
    bool operator<=(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return _lhs.get() <= _rhs.get();
    }

    template<typename _T, typename _U>
    bool operator>(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return _lhs.get() > _rhs.get();
    }

    template<typename _T, typename _U>
    bool operator>=(const LiteSharedPtr<_T>& _lhs, const LiteSharedPtr<_U>& _rhs) noexcept {
        return _lhs.get() >= _rhs.get();
    }

    template<typename _T>
    bool operator==(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return _lhs.get() == nullptr;
    }

    template<typename _T>
    bool operator==(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return nullptr == _rhs.get();
    }

    template<typename _T>
    bool operator!=(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return _lhs.get() != nullptr;
    }

    template<typename _T>
    bool operator!=(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return nullptr != _rhs.get();
    }

    template<class _T>
    bool operator<(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return (_lhs.get() < static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr));
    }

    template<class _T>
    bool operator<(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return (static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr) < _rhs.get());
    }

    template<class _T>
    bool operator<=(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return (_lhs.get() <= static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr));
    }

    template<class _T>
    bool operator<=(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return (static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr) <= _rhs.get());
    }

    template<class _T>
    bool operator>(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return (_lhs.get() > static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr));
    }

    template<class _T>
    bool operator>(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return (static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr) > _rhs.get());
    }

    template<class _T>
    bool operator>=(const LiteSharedPtr<_T>& _lhs, std::nullptr_t) noexcept {
        return (_lhs.get() >= static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr));
    }

    template<class _T>
    bool operator>=(std::nullptr_t, const LiteSharedPtr<_T>& _rhs) noexcept {
        return (static_cast<typename LiteSharedPtr<_T>::element_type *>(nullptr) >= _rhs.get());
    }


}

namespace std {
    template<typename _Ty>
    void swap(Mix::LiteSharedPtr<_Ty>& _lhs, Mix::LiteSharedPtr<_Ty>& _rhs) noexcept {
        _lhs.swap(_rhs);
    }

    template<class _Ty>
    struct hash<Mix::LiteSharedPtr<_Ty>> {
        typedef Mix::LiteSharedPtr<_Ty> argument_type;
        typedef size_t result_type;

        size_t operator()(const Mix::LiteSharedPtr<_Ty>& _key) const noexcept {
            return (hash<typename Mix::LiteSharedPtr<_Ty>::element_type *>()(_key.get()));
        }
    };

}

#endif

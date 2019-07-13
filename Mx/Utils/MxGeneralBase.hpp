#pragma once

#ifndef MX_GENERAL_BASE_H_
#define MX_GENERAL_BASE_H_

#include "../Exceptions/MxExceptions.hpp"
#include <memory>

namespace Mix::GeneralBase {
    /** @brief Base for classes that are not copyable */
    struct NoCopyBase {
        NoCopyBase(const NoCopyBase&) = delete;
        NoCopyBase& operator=(const NoCopyBase&) = delete;

    protected:
        NoCopyBase() = default;
        ~NoCopyBase() = default;

        NoCopyBase(NoCopyBase&&) = default;
        NoCopyBase& operator=(NoCopyBase&&) = default;
    };

    struct NoCopyAndMoveBase {
        NoCopyAndMoveBase(const NoCopyAndMoveBase&) = delete;

    protected:
        NoCopyAndMoveBase() = default;
        ~NoCopyAndMoveBase() = default;
    };

#define MAKE_CLASS_STATIC(ClsName)\
    public: ClsName() = delete; ClsName(const ClsName&) = delete; private:

    /** @brief Base for static class */
    struct StaticBase : NoCopyAndMoveBase {
        StaticBase() = delete;
    };

    /**
     * @brief Base for singleton
     * 
     * @b Example:
     * @code
     * class ClassName : public SingletonBase<ClassName> {
     *     friend SingletonBase<ClassName>;
     * public:
     *     ~ClassName() {...} // Destructor must be public
     * private:
     *     ClassName(...) {...} // Constructor should be private
     *  }
     *  @endcode
     */
    template<typename _Ty>
    class SingletonBase {
    public:
        template<typename... _Args>
        static void Initialize(const std::string& _name, _Args&&... _args) {
            static bool firstCall = true;
            if(firstCall) {
                sInstancePtr.reset(new _Ty(std::forward<_Args>(_args)...));
                sName = _name;
                firstCall = false;
            }
        }

        static _Ty& Instance() {
            // no exception thrown here
            //if(!sInstancePtr)
            //    throw Exception("Error : [ %s ] been called before initialized", sName.c_str());
            return *sInstancePtr;
        }

        SingletonBase(const SingletonBase&) = delete;

        // Deleting the copy ctor ensures the followings
        //SingletonBase(SingletonBase&&) noexcept = delete;
        //SingletonBase& operator=(const SingletonBase&) = delete;
        //SingletonBase& operator=(SingletonBase&&) noexcept = delete;

    protected:
        SingletonBase() = default;

        ~SingletonBase() = default;

        static std::string sName;

        static std::unique_ptr<_Ty> sInstancePtr;
    };

    template<typename _Ty>
    std::string SingletonBase<_Ty>::sName;

    template<typename _Ty>
    std::unique_ptr<_Ty> SingletonBase<_Ty>::sInstancePtr = nullptr;
}

#endif

#pragma once
#ifndef MX_GENERAL_BASE_H_
#define MX_GENERAL_BASE_H_
#include <memory>
#include "../Exceptions/MxExceptions.hpp"

namespace Mix {
    namespace GeneralBase {
        /**
         * @brief Base for classes that is non-copyable
         */
        struct NoCopyBase {
            NoCopyBase() = default;

            ~NoCopyBase() = default;

            NoCopyBase(const NoCopyBase&) = delete;

            NoCopyBase(NoCopyBase&&) = default;

            NoCopyBase& operator=(const NoCopyBase&) = delete;

            NoCopyBase& operator=(NoCopyBase&&) = default;
        };

        /**
         * @brief Base for static class
         */
        struct StaticBase {
            StaticBase() = delete;

            ~StaticBase() = delete;

            StaticBase(const StaticBase&) = delete;

            StaticBase(StaticBase&&) = delete;

            StaticBase& operator=(const StaticBase&) = delete;

            StaticBase& operator=(StaticBase&&) = delete;
        };

        /**
         * @brief Base for singleton
         * @example class ClassName:public SingletonBase<ClassName>{
         *                  friend SingletonBase<ClassName>;
         *              public:
         *                  ~ClassName(){} // Destructor must be public
         *              private:
         *                  ClassName(...){} //Constructor should be private
         *              }
         */
        template<typename _Ty>
        class SingletonBase :public NoCopyBase {
        public:
            template<typename ... _Args>
            static void Initialize(const std::string& _name, _Args&&... _args) {
                static bool firstCall = true;
                if (firstCall) {
                    sInstancePtr.reset(new _Ty(std::forward<_Args>(_args)...));
                    sName = _name;
                    firstCall = false;
                }
            }

            static _Ty& Instance() {
                if (!sInstancePtr)
                    throw Exception("Error : [ %s ] been called before initialized", sName.c_str());
                return *sInstancePtr;
            }

            SingletonBase(const SingletonBase& _other) = delete;

            SingletonBase(SingletonBase&& _other) noexcept = delete;

            SingletonBase& operator=(const SingletonBase& _other) = delete;

            SingletonBase& operator=(SingletonBase&& _other) noexcept = delete;

        private:
            SingletonBase() = default;

            ~SingletonBase() = default;

            static std::string sName;

            static std::unique_ptr<_Ty> sInstancePtr;
        };

        template<typename _Ty>
        std::string SingletonBase<_Ty>::sName;

        template<typename _Ty>
        std::unique_ptr<_Ty> SingletonBase<_Ty>::sInstancePtr = nullptr;

        class Updateable {
        public:
            virtual ~Updateable() = default;

            virtual void update() = 0;

            virtual void fixUpdate() = 0;

            virtual void lateUpdate() = 0;
        };
    }
}

#endif

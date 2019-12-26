#pragma once

#ifndef MX_RTTI_HPP_
#define MX_RTTI_HPP_

#include <string>
#include <utility>
#include <type_traits>

namespace Mix {
    class Object;

    class Rtti {
    public:
        /**
         *  @brief Create a rtti instance
         *  @param _rttiName The name of class that owns this Rtti instance
         *  @param _pBase The rtti of the base class
         */
        Rtti(std::string _rttiName, const Rtti* _pBase) :
            mRttiName(std::move(_rttiName)),
            mpBase(_pBase) {
        }

        ~Rtti() = default;

        /**
         *  @brief Get the name of the class that owns this rtti
         *  @return The name of the class
         */
        const std::string& getName() const { return mRttiName; };

        /**
         *  @brief Check if two rtti stands for the same type
         *  @param _type Another rtti
         */
        bool isSameType(const Rtti& _type) const { return (&_type == this); }

        /**
         *  @brief Check if two rtti stands for the same type
         *  @param _type Another rtti
         */
        bool isSameType(const Rtti* _type) const { return (_type == this); }

        /**
         *  @brief Check if this class is derived from another
         *  @param _type Rtti of another class
         *  @return If this class is derived from the class that owns _type
         */
        bool isDerivedFrom(const Rtti& _type) const { return isDerivedFrom(&_type); }

        bool isDerivedFrom(const Rtti* _type) const;

        /**
         *  @return Base class of this class, otherwise return nullptr.
         */
        const Rtti* getBase() const { return mpBase; }

    private:
        std::string mRttiName;
        const Rtti* mpBase;
    };
}

#define MX_DECLARE_RTTI \
public:\
    static Mix::Rtti msType;\
    virtual Mix::Rtti& getType() const { return msType; }\
    static Mix::Rtti& GetType();\
private:

#define MX_IMPLEMENT_RTTI(className,baseClassName)\
static_assert(std::is_base_of<baseClassName, className>::value, "["#baseClassName"] should be base of ["#className"]");\
Mix::Rtti className::msType(#className, &baseClassName::msType);\
Mix::Rtti& className::GetType() {return msType;}

//#define MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(className, baseClassName)\
//Mix::Rtti className::msType(#className, &baseClassName::msType, nullptr);

#define MX_IMPLEMENT_RTTI_NOPARENT(className)\
Mix::Rtti className::msType(#className, nullptr);

//#define MX_IMPLEMENT_RTTI_NO_PARENT_NO_CREATE_FUNC(className)\
//Mix::Rtti className::msType(#className, nullptr, nullptr);

#endif

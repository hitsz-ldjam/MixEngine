#pragma once

#ifndef MX_RTTI_HPP
#define MX_RTTI_HPP

#include <string>
#include <utility>

namespace Mix {
    class Object;

    typedef Object*(*createObjectFunc)();

    class Rtti {
    public:
        /** 
         *  @brief Create a rtti instance
         *  @param _rttiName The name of class that owns this Rtti instance
         *  @param _pBase The rtti of the base class
         *  @param _COF Unused parameter
         */
        Rtti(std::string _rttiName, const Rtti* _pBase, createObjectFunc _COF): mRttiName(std::move(_rttiName)),
                                                                                mpBase(_pBase) {}

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

        bool isDerivedFrom(const Rtti* _type) const {
            const Rtti* pTemp = this;

            if(pTemp->isSameType(_type))
                return false;

            while(!pTemp->isSameType(_type)) {
                if(pTemp->mpBase) {
                    pTemp = mpBase;
                }
                else {
                    return false;
                }
            }
            return true;
        }

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
    static Rtti msType;\
    virtual Rtti& getType() const { return msType; }

// deprecated
//#define MX_IMPLEMENT_RTTI(className,baseClassName)\
//Rtti className::msType(#className, &baseClassName::msType, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(className, baseClassName)\
Rtti className::msType(#className, &baseClassName::msType, nullptr);

// deprecated
//#define MX_IMPLEMENT_RTTI_NoParent(className)\
//Rtti className::msType(#className, nullptr, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NO_PARENT_NO_CREATE_FUNC(className)\
Rtti className::msType(#className, nullptr, nullptr);

#endif

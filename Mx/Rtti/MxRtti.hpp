#pragma once
#ifndef _MX_RTTI_HPP_
#define _MX_RTTI_HPP_

#include<string>

namespace Mix {

    class Object;

    typedef Object*(*createObjectFunc)();

    class Rtti {
    private:
        std::string mRttiName;
        const Rtti* mpBase;

    public:
        /** @param COF Unused. */
        Rtti(const std::string& rttiName, const Rtti* pBase, createObjectFunc COF)
            :mRttiName(rttiName),
            mpBase(pBase) {
        };

        ~Rtti() = default;

        const std::string& getName() const { return mRttiName; };

        inline bool isSameType(const Rtti& type) const { return (&type == this); };
        inline bool isSameType(const Rtti* type) const { return (type == this); };

        inline bool isDerived(const Rtti& type) const { isDerived(&type); };
        bool isDerived(const Rtti* type) const {
            const Rtti* pTemp = this;

            if (pTemp->isSameType(type))
                return false;

            while (!pTemp->isSameType(type)) {
                if (pTemp->mpBase) {
                    pTemp = mpBase;
                } else {
                    return false;
                }
            }
            return true;
        }

        inline const Rtti* getBase() const { return mpBase; };

    };
}

#define MX_DECLARE_RTTI \
public:\
    static Rtti msType;\
    virtual Rtti& getType() const {return msType;};

// todo
//#define MX_IMPLEMENT_RTTI(className,baseClassName)\
//Rtti className::msType(#className, &baseClassName::msType, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NoCreateFunc(className,baseClassName)\
Rtti className::msType(#className, &baseClassName::msType, nullptr);

//#define MX_IMPLEMENT_RTTI_NoParent(className)\
//Rtti className::msType(#className, nullptr, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NoParent_NoCreateFunc(className)\
Rtti className::msType(#className, nullptr, nullptr);

#endif // !_MX_RTTI_HPP_

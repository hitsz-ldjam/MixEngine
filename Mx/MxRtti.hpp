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
        
        /**
         * \brief Create a Rtti instance
         * \param _rttiName The name of class that owns this Rtti instance
         * \param _pBase The Rtti of the base class
         * \param COF Not used now
         */
        Rtti(const std::string& _rttiName, const Rtti* _pBase, createObjectFunc COF)
            :mRttiName(_rttiName),
            mpBase(_pBase) {
        };

        ~Rtti() = default;


        /**
         * \brief Get the name of the Class that owns this Rtti
         * \return The name of the class
         */
        const std::string& GetName() const { return mRttiName; };

        /**
         * \brief Check if two Rtti stands for the same type
         * \param _type Another Rtti
         */
        bool IsSameType(const Rtti& _type) const { return (&_type == this); }

        /**
         * \brief Check if two Rtti stands for the same type
         * \param _type Another Rtti
         */
        bool IsSameType(const Rtti* _type) const { return (_type == this); }

        /**
         * \brief Check if one Class is derived from another
         * \param _type Rtti of another Class
         * \return If this Class is derived from the Class that owns _type
         */
        bool IsDerivedFrom(const Rtti& _type) const { return IsDerivedFrom(&_type); }

        bool IsDerivedFrom(const Rtti* _type) const {
            const Rtti* pTemp = this;

            if (pTemp->IsSameType(_type))
                return false;

            while (!pTemp->IsSameType(_type)) {
                if (pTemp->mpBase) {
                    pTemp = mpBase;
                } else {
                    return false;
                }
            }
            return true;
        }

        /**
         * \brief Get the base Class of this class, return nullptr if no base Class
         */
        const Rtti* GetBase() const { return mpBase; };

    };
}

#define MX_DECLARE_RTTI \
public:\
    static Rtti msType;\
    virtual Rtti& GetType() const {return msType;};

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

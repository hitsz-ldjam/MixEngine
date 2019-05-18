#pragma once
#ifndef _MX_RTTI_HPP_
#define _MX_RTTI_HPP_

#include<string>
#include <utility>

namespace Mix {

    class Object;

    typedef Object*(*CreateObjectFunc)();

    class Rtti {
    private:
        std::string mRttiName;
        const Rtti* mBase;

    public:
        
        /**
         * \brief Create a Rtti instance
         * \param _rttiName The name of class that owns this Rtti instance
         * \param _pBase The Rtti of the base class
         * \param _cof Not used now
         */
        Rtti(std::string _rttiName, const Rtti* _pBase, CreateObjectFunc _cof)
            :mRttiName(std::move(_rttiName)),
            mBase(_pBase) {
        };

        ~Rtti() = default;


        /**
         * \brief Get the name of the Class that owns this Rtti
         * \return The name of the class
         */
        const std::string& getName() const { return mRttiName; };

        /**
         * \brief Check if two Rtti stands for the same type
         * \param _type Another Rtti
         */
        bool isSameType(const Rtti& _type) const { return (&_type == this); }

        /**
         * \brief Check if two Rtti stands for the same type
         * \param _type Another Rtti
         */
        bool isSameType(const Rtti* _type) const { return (_type == this); }

        /**
         * \brief Check if one Class is derived from another
         * \param _type Rtti of another Class
         * \return If this Class is derived from the Class that owns _type
         */
        bool isDerivedFrom(const Rtti& _type) const { return isDerivedFrom(&_type); }

        bool isDerivedFrom(const Rtti* _type) const {
            const Rtti* pTemp = this;

            if (pTemp->isSameType(_type))
                return false;

            while (!pTemp->isSameType(_type)) {
                if (pTemp->mBase) {
                    pTemp = mBase;
                } else {
                    return false;
                }
            }
            return true;
        }

        /**
         * \brief Get the base Class of this class, return nullptr if no base Class
         */
        const Rtti* getBase() const { return mBase; };

    };
}

#define MX_DECLARE_RTTI \
public:\
    static Rtti Type;\
    virtual Rtti& GetType() const {return Type;};

// todo
//#define MX_IMPLEMENT_RTTI(className,baseClassName)\
//Rtti className::msType(#className, &baseClassName::msType, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(className,baseClassName)\
Rtti className::Type(#className, &baseClassName::Type, nullptr);

//#define MX_IMPLEMENT_RTTI_NoParent(className)\
//Rtti className::msType(#className, nullptr, className::factoryFunc);

#define MX_IMPLEMENT_RTTI_NO_PARENT_NO_CREATE_FUNC(className)\
Rtti className::Type(#className, nullptr, nullptr);

#endif // !_MX_RTTI_HPP_

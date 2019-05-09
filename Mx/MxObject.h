#pragma once
#ifndef _MX_OBJECT_H_
#define _MX_OBJECT_H_

#include "MxDef.h"
#include "MxRtti.hpp"
#include <map>
#include <memory>
#include <vector>
#include <iostream>

#define MX_DECLARE_CLASS_FACTORY \
public:\
    static const bool msClassFactoryRegistered;\
private:\
    static Object* factoryFunction();

#define MX_DECLARE_NO_CLASS_FACTORY \
public:\
    static const bool msClassFactoryRegistered = false;

#define MX_IMPLEMENT_CLASS_FACTORY(className)\
    const bool className::msClassFactoryRegistered=\
        className::RegisterFactoryFunc(#className,className::factoryFunction);

#define MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(className)\
Object* className::factoryFunction(){\
    return new className();\
}\
MX_IMPLEMENT_CLASS_FACTORY(className)\


namespace Mix {

    typedef Object*(*FactoryFunction)();

    struct GlobalClassFactoryFuncMap {

        static std::weak_ptr <std::map<std::string, FactoryFunction>> getInstance() {
            static std::shared_ptr<std::map<std::string, FactoryFunction>> instance = nullptr;

            if (!instance)
                instance = std::make_shared<std::map<std::string, FactoryFunction>>();

            return instance;
        }
    };


    class Object {
        MX_DECLARE_RTTI;
        MX_DECLARE_NO_CLASS_FACTORY;
    public:
        Object() {
            AddObject(this);
        }

        virtual ~Object() = 0 {};
        
        /**
         * \brief Check if this is the same type as _type
         */
        bool IsSameType(const Rtti& _type) const { return(&GetType() == &_type); }

        /**
         * \brief Check if this is the same type as _type
         */
        bool IsSameType(const Rtti* _type) const { return (&GetType() == _type); }

        /**
         * \brief Check if this is the same type as _object
         */
        bool IsSameType(const Object& _object) const { return (&GetType() == &_object.GetType()); }

        /**
         * \brief Check if this is the same type as _object
         */
        bool IsSameType(const Object* _object) const { return (&GetType() == &_object->GetType()); }

        /**
         * \brief Check if this is derived from the Class that owns _type
         */
        bool IsDerived(const Rtti& _type) const { return IsDerived(&_type); }

        /**
         * \brief Check if this is derived from the Class that owns _type
         */
        bool IsDerived(const Rtti* _type) const { return GetType().IsDerivedFrom(_type); }

        /**
         * \brief Check if this is derived from _object
         */
        bool IsDerived(const Object& _object) const { return IsDerived(&_object); }

        /**
         * \brief Check if this is derived from _object
         */
        bool IsDerived(const Object* _object) const { return GetType().IsDerivedFrom(&_object->GetType()); }


        /**
         * \brief Get the type name of this
         */
        const std::string& GetTypeName() const { return GetType().GetName(); }

        /**
         * \brief Get the name of this
         */
        const std::string& GetName() const { return mName; }

    protected:
        std::string mName;

        //static
    public:

        /**
         * \brief Find an object of T type, return nullptr if there is no one exists
         */
        template<typename T>
        static T* FindObjectOfType();

        /**
         * \brief Find all objects of T type
         */
        template<typename T>
        static std::vector<T*> FindObjectsOfType();

    protected:
        /**
         * \brief Register factory function to global factory function map
         * \note Not used now
         */
        static bool RegisterFactoryFunc(const std::string& _typeName, FactoryFunction _func);

    private:
        static std::vector<Object*> mObjectList;

        /**
         * \brief Add an object to global Object list
         */
        static void AddObject(Object* _obj) {
            mObjectList.push_back(_obj);
        }

        /**
         * \brief Remove an object from global Object list
         */
        static void RemoveObject(Object* _obj) {
            const auto it = std::find(mObjectList.begin(), mObjectList.end(), _obj);
            if (it == mObjectList.end())
                return;
            mObjectList.erase(it);
        }
    };

    template<typename T>
    T * Object::FindObjectOfType() {
        T* ptr;

        for (auto obj : mObjectList) {
            // check the type of Object
            if (ptr = dynamic_cast<T*>(obj))
                return ptr;
        }

        return nullptr;
    }

    template<typename T>
    inline std::vector<T*> Object::FindObjectsOfType() {
        std::vector<T*> results;
        T* ptr;

        for (auto obj : mObjectList) {
            // check the type of comp
            if (ptr = dynamic_cast<T*>(obj))
                results.push_back(ptr);
        }

        return results;
    }
}

#endif // !_MX_OBJECT_H_

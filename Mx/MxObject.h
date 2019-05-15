#pragma once
#ifndef MX_OBJECT_H_
#define MX_OBJECT_H_

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

        static std::weak_ptr <std::map<std::string, FactoryFunction>> GetInstance() {
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

        virtual ~Object() = 0 {
            RemoveObject(this);
        };

        Object(const Object& _other) = default;

        Object(Object&& _other) noexcept = default;

        Object& operator=(const Object& _other) = default;

        Object& operator=(Object&& _other) noexcept = default;

        /**
         * \brief Check if this is the same type as _type
         */
        bool isSameType(const Rtti& _type) const { return(&GetType() == &_type); }

        /**
         * \brief Check if this is the same type as _type
         */
        bool isSameType(const Rtti* _type) const { return (&GetType() == _type); }

        /**
         * \brief Check if this is the same type as _object
         */
        bool isSameType(const Object& _object) const { return (&GetType() == &_object.GetType()); }

        /**
         * \brief Check if this is the same type as _object
         */
        bool isSameType(const Object* _object) const { return (&GetType() == &_object->GetType()); }

        /**
         * \brief Check if this is derived from the Class that owns _type
         */
        bool isDerived(const Rtti& _type) const { return isDerived(&_type); }

        /**
         * \brief Check if this is derived from the Class that owns _type
         */
        bool isDerived(const Rtti* _type) const { return GetType().isDerivedFrom(_type); }

        /**
         * \brief Check if this is derived from _object
         */
        bool isDerived(const Object& _object) const { return isDerived(&_object); }

        /**
         * \brief Check if this is derived from _object
         */
        bool isDerived(const Object* _object) const { return GetType().isDerivedFrom(&_object->GetType()); }


        /**
         * \brief Get the type name of this
         */
        const std::string& getTypeName() const { return GetType().getName(); }

        /**
         * \brief Get the name of this
         */
        const std::string& getName() const { return mName; }

    protected:
        std::string mName;

        //static
    public:

        /**
         * \brief Find an object of T type, return nullptr if there is no one exists
         */
        template<typename _Ty>
        static _Ty* FindObjectOfType();

        /**
         * \brief Find all objects of T type
         */
        template<typename _Ty>
        static std::vector<_Ty*> FindObjectsOfType();

    protected:
        /**
         * \brief Register factory function to global factory function map
         * \note Not used now
         */
        static bool RegisterFactoryFunc(const std::string& _typeName, FactoryFunction _func);

    private:
        static std::vector<Object*> sObjectList;

        /**
         * \brief Add an object to global Object list
         */
        static void AddObject(Object* _obj) {
            sObjectList.push_back(_obj);
        }

        /**
         * \brief Remove an object from global Object list
         */
        static void RemoveObject(Object* _obj) {
            const auto it = std::find(sObjectList.begin(), sObjectList.end(), _obj);
            if (it == sObjectList.end())
                return;
            sObjectList.erase(it);
        }
    };

    template<typename _Ty>
    _Ty * Object::FindObjectOfType() {
        _Ty* ptr;

        for (auto obj : sObjectList) {
            // check the type of Object
            if ((ptr = dynamic_cast<_Ty*>(obj)))
                return ptr;
        }

        return nullptr;
    }

    template<typename _Ty>
    std::vector<_Ty*> Object::FindObjectsOfType() {
        std::vector<_Ty*> results;
        _Ty* ptr;

        for (auto obj : sObjectList) {
            // check the type of comp
            if ((ptr = dynamic_cast<_Ty*>(obj)))
                results.push_back(ptr);
        }

        return results;
    }
}

#endif // !MX_OBJECT_H_

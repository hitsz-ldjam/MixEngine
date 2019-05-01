#pragma once
#ifndef _MX_OBJECT_H_
#define _MX_OBJECT_H_

#include "MxDef.h"
#include "MxRtti.hpp"
#include "MxException.hpp"
#include <iostream>
#include <map>
#include <memory>
#include <vector>


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
        className::registerFactoryFunc(#className,className::factoryFunction);

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
            addObject(this);
        }
        virtual ~Object() = 0 {};
        
    public:
        bool isSameType(const Rtti& type) const { return(&getType() == &type); };
        bool isSameType(const Rtti* pType) const { return (&getType() == pType); }
        bool isSameType(const Object& object) const { return (&getType() == &object.getType()); };
        bool isSameType(const Object* pObject) const { return (&getType() == &pObject->getType()); };

        bool isDerived(const Rtti& type) const { return isDerived(&type); };
        bool isDerived(const Rtti* pType) const { return getType().isDerived(pType); };
        bool isDerived(const Object& object) const { return isDerived(&object); };
        bool isDerived(const Object* pObject) const { return getType().isDerived(&pObject->getType()); };

        const std::string& getTypeName() const { return getType().getName(); };

    protected:
        std::string mName;

        //static
    public:
        template<typename T>
        static T* findObjectOfType();

        template<typename T>
        static std::vector<T*> findObjectsOfType();

    protected:
        static bool registerFactoryFunc(const std::string& typeName, FactoryFunction func);

    private:
        static std::vector<Object*> mObjectList;
        static void addObject(Object* obj) {
            mObjectList.push_back(obj);
        }

        static void removeObject(Object* obj) {
            auto it = std::find(mObjectList.begin(), mObjectList.end(), obj);
            if (it == mObjectList.end())
                return;
            mObjectList.erase(it);
        }
    };

    template<typename T>
    T * Object::findObjectOfType() {
        T* ptr;

        for (auto obj : mObjectList) {
            // check the type of Object
            if (ptr = dynamic_cast<T*>(obj))
                return ptr;
        }

        return nullptr;
    }

    template<typename T>
    inline std::vector<T*> Object::findObjectsOfType() {
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

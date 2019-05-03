#pragma once
#ifndef _MX_OBJECT_H_
#define _MX_OBJECT_H_

#include "MxDef.h"
#include "MxRtti.hpp"
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
        
    public:
        bool IsSameType(const Rtti& _type) const { return(&getType() == &_type); }
        bool IsSameType(const Rtti* _pType) const { return (&getType() == _pType); }
        bool IsSameType(const Object& _object) const { return (&getType() == &_object.getType()); }
        bool IsSameType(const Object* _pObject) const { return (&getType() == &_pObject->getType()); }

        bool IsDerived(const Rtti& _type) const { return IsDerived(&_type); }
        bool IsDerived(const Rtti* _pType) const { return getType().isDerived(_pType); }
        bool IsDerived(const Object& _object) const { return IsDerived(&_object); }
        bool IsDerived(const Object* _pObject) const { return getType().isDerived(&_pObject->getType()); }

        const std::string& GetTypeName() const { return getType().getName(); }

        const std::string& GetName() const { return mName; }

    protected:
        std::string mName;

        //static
    public:
        template<typename T>
        static T* FindObjectOfType();

        template<typename T>
        static std::vector<T*> FindObjectsOfType();

    protected:
        static bool RegisterFactoryFunc(const std::string& _typeName, FactoryFunction _func);

    private:
        static std::vector<Object*> mObjectList;
        static void AddObject(Object* _obj) {
            mObjectList.push_back(_obj);
        }

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

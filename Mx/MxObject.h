#pragma once
#ifndef _MX_OBJECT_H_
#define _MX_OBJECT_H_

#include "MxRtti.hpp"
#include "MxException.hpp"
#include <iostream>
#include <map>
#include <memory>


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
        virtual ~Object() = 0 {};
    protected:
        Object() = default;

        static bool registerFactoryFunc(const std::string& typeName, FactoryFunction func);

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
    };
}

#endif // !_MX_OBJECT_H_

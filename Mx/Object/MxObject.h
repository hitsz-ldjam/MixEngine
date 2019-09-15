#pragma once

#ifndef MX_OBJECT_H_
#define MX_OBJECT_H_

#include <map>
#include <memory>
#include <vector>

#include "../Rtti/MxRtti.hpp"

namespace Mix {
    class Object {
        MX_DECLARE_RTTI;
    public:
        Object() = default;

        virtual ~Object() = default;

        Object(const Object& _other) = default;

        Object(Object&& _other) = default;

        Object& operator=(const Object& _other) = default;

        Object& operator=(Object&& _other) = default;

        /**
         *  @brief Check if this is the same type as _type
         */
        bool isSameType(const Rtti& _type) const { return (&getType() == &_type); }

        /**
         *  @brief Check if this is the same type as _type
         */
        bool isSameType(const Rtti* _type) const { return (&getType() == _type); }

        /**
         *  @brief Check if this is the same type as _object
         */
        bool isSameType(const Object& _object) const { return (&getType() == &_object.getType()); }

        /**
         *  @brief Check if this is the same type as _object
         */
        bool isSameType(const Object* _object) const { return (&getType() == &_object->getType()); }

        /**
         *  @brief Check if this is derived from the Class that owns _type
         */
        bool isDerived(const Rtti& _type) const { return isDerived(&_type); }

        /**
         *  @brief Check if this is derived from the Class that owns _type
         */
        bool isDerived(const Rtti* _type) const { return getType().isDerivedFrom(_type); }

        /**
         *  @brief Check if this is derived from _object
         */
        bool isDerived(const Object& _object) const { return isDerived(&_object); }

        /**
         *  @brief Check if this is derived from _object
         */
        bool isDerived(const Object* _object) const { return getType().isDerivedFrom(&_object->getType()); }

        /**
         *  @brief Get the type name of this
         */
        const std::string& getTypeName() const { return getType().getName(); }

        // ----- static variables and functions -----

    //public:

    //    /**
    //     *  @brief Find an object of T type, return nullptr if there is no one exists
    //     */
    //    template <typename T>
    //    static T* FindObjectOfType();

    //    /**
    //     *  @brief Find all objects of T type
    //     */
    //    template <typename T>
    //    static std::vector<T*> FindObjectsOfType();

    //protected:
    //    /**
    //     *  @brief Register factory function to global factory function map
    //     *  @note Not used now
    //     */
    //    static bool RegisterFactoryFunc(const std::string& _typeName, FactoryFunction _func);

    //private:
    //    static std::vector<Object*> sObjectList;

    //    /**
    //     *  @brief Add an object to global Object list
    //     */
    //    static void AddObject(Object* _obj) {
    //        sObjectList.push_back(_obj);
    //    }

    //    /**
    //     *  @brief Remove an object from global Object list
    //     */
    //    static void RemoveObject(Object* _obj) {
    //        const auto it = std::find(sObjectList.begin(), sObjectList.end(), _obj);
    //        if (it == sObjectList.end())
    //            return;
    //        sObjectList.erase(it);
    //    }

    //};

    //template <typename T>
    //T* Object::FindObjectOfType() {
    //    T* ptr;

    //    for (auto obj : sObjectList) {
    //        if ((ptr = dynamic_cast<T*>(obj)))
    //            return ptr;
    //    }

    //    return nullptr;
    //}

    //template <typename T>
    //std::vector<T*> Object::FindObjectsOfType() {
    //    std::vector<T*> results;
    //    T* ptr;

    //    for (auto obj : sObjectList) {
    //        if ((ptr = dynamic_cast<T*>(obj)))
    //            results.push_back(ptr);
    //    }

    //    return results;
    //}
    };
}

#endif

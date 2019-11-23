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
    };
}

#endif

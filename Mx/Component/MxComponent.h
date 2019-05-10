#pragma once

#ifndef MX_COMPONENT_H
#define MX_COMPONENT_H

#include "../Object/MxObject.h"

namespace Mix {
    class GameObject;

    class Component : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_NO_CLASS_FACTORY

    public:
        Component() : mGameObject(nullptr) {}
        virtual ~Component() = 0 {}

        void setGameObject(GameObject* _gameObject) {
            mGameObject = _gameObject;
        }

        GameObject* getGameObject() const {
            return mGameObject;
        }

        //virtual Component* copy() const = 0;

    protected:
        GameObject* mGameObject;
    };
}

#endif

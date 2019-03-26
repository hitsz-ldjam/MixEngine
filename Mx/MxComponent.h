#pragma once
#ifndef _MX_COMPONENT_H_
#define _MX_COMPONENT_H_


#include "MxObject.h"
#include <string>


namespace Mix {
    class GameObject;

    class Component : public Object {
        MX_DECLARE_RTTI;
        MX_DECLARE_NO_CLASS_FACTORY;
    public:
        Component() :mGameObj(nullptr) {};
        virtual ~Component() = 0 {};

        void setGameObj(GameObject* gameObj) {
            mGameObj = gameObj;
        }

        GameObject* getGameObj() {
            return mGameObj;
        }

        virtual Component* copy() const = 0;

    protected:
        GameObject* mGameObj;
    };

}

#endif

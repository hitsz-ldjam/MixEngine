#pragma once
#ifndef MX_COMPONENT_H_
#define MX_COMPONENT_H_


#include "MxObject.h"


namespace Mix {
    class GameObject;
    class Transform;

    class Component : public Object {
        MX_DECLARE_RTTI;
        MX_DECLARE_NO_CLASS_FACTORY;
    public:
        Component() :mGameObj(nullptr) {};
        virtual ~Component() = 0 {};

        void setGameObj(GameObject* _gameObj) {
            mGameObj = _gameObj;
        }

        GameObject* getGameObj() const {
            return mGameObj;
        }

        //virtual Component* copy() const = 0;

    protected:
        GameObject* mGameObj;
    };

}

#endif

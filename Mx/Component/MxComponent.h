#pragma once

#ifndef MX_COMPONENT_H_
#define MX_COMPONENT_H_

#include "../Object/MxObject.h"

namespace Mix {
    class GameObject;

    class Component : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_NO_CLASS_FACTORY

        friend GameObject;

    public:
        Component() : mGameObject(nullptr) {}
        virtual ~Component() = 0 {}
        auto getGameObject() const noexcept { return mGameObject; }

    protected:
        GameObject* mGameObject;
        void setGameObject(GameObject* _obj) noexcept { mGameObject = _obj; }
    };
}

#endif

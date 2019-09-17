#pragma once

#ifndef MX_BEHAVIOUR_H_
#define MX_BEHAVIOUR_H_

#include "../MxComponent.h"

namespace Mix {
    class Behaviour : public Component {
        MX_DECLARE_RTTI;
        friend class GameObject;
    public:
        Behaviour() = default;
        virtual ~Behaviour() = 0 {}

    private:
        virtual void awake() {}
        virtual void init() {}
        virtual void update() {}
        virtual void fixedUpdate() {}
        virtual void lateUpdate() {}
    };
}

#endif

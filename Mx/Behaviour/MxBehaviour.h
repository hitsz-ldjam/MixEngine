#pragma once

#ifndef MX_BEHAVIOUR_H
#define MX_BEHAVIOUR_H

#include "../Object/MxObject.h"

namespace Mix {
    class Behaviour : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_NO_CLASS_FACTORY

    public:
        Behaviour() = default;
        virtual ~Behaviour() = 0 {}

        virtual void init() {}
        virtual void update() {}

        // todo
        //virtual void fixedUpdate() {}
    };
}

#endif

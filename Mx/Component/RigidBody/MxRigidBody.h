#pragma once

#ifndef MX_RIGID_BODY_H_
#define MX_RIGID_BODY_H_

#include "../Behaviour/MxBehaviour.h"

namespace Mix {
    // todo:
    class RigidBody final : public Behaviour {
        MX_DECLARE_RTTI
        MX_DECLARE_CLASS_FACTORY

    public:
        RigidBody() = default;
        ~RigidBody() = default;
    };
}

#endif

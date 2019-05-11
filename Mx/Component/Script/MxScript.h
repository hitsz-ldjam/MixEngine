#pragma once

#ifndef MX_SCRIPT_H
#define MX_SCRIPT_H

#include "../Behaviour/MxBehaviour.h"

namespace Mix {
    class Script : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        Script() = default;
        virtual ~Script() = 0 {}
    };
}

#endif

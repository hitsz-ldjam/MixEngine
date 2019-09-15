#pragma once

#ifndef MX_SCRIPT_H_
#define MX_SCRIPT_H_

#include "../Behaviour/MxBehaviour.h"

namespace Mix {
    class Script : public Behaviour {
    MX_DECLARE_RTTI

    public:
        Script() = default;
        virtual ~Script() = 0 {}
    };
}

#endif

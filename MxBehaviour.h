#pragma once

#ifndef _MX_BEHAVIOUR_H_
#define _MX_BEHAVIOUR_H_

#include "MxEvent.h"

namespace Mix {
    class MxBehaviour : public MxEvent {
    public:
        MxBehaviour() {};
        virtual ~MxBehaviour() {};
        virtual void init() {};
        virtual void update() {};
    };
}

#endif

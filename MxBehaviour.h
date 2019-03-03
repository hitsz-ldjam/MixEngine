#pragma once

#ifndef _MX_BEHAVIOUR_H_
#define _MX_BEHAVIOUR_H_

#include "MxEvent.h"

namespace Mix {
    class Behaviour : public Event {
    public:
        Behaviour() {};
        virtual ~Behaviour() {};
        virtual void init() {};
        virtual void update() {};
    };
}

#endif

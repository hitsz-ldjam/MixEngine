#pragma once

#ifndef _MX_BEHAVIOUR_H_
#define _MX_BEHAVIOUR_H_

#include "MxComponent.h"

namespace Mix {
    class Behaviour : public Component {
    public:
        Behaviour() = default;
        virtual ~Behaviour() = default;

        virtual void fixedUpdate() {}
        virtual void init() {}
        virtual void lateUpdate() {}
        virtual void update() {}

        /** @return If false, the application will NOT exit. */
        virtual bool onApplicationQuit() { return true; }
        virtual void onPostRender() {}
        virtual void onPreRender() {}

        // todo: handle other events
    };
}

#endif

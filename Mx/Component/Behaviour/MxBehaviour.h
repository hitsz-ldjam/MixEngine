#pragma once

#ifndef MX_BEHAVIOUR_H_
#define MX_BEHAVIOUR_H_

#include "../MxComponent.h"

namespace Mix {
    class Behaviour : public Component {
        MX_DECLARE_RTTI;
        friend class GameObject;
        friend class Scene;
    public:
        Behaviour() = default;
        virtual ~Behaviour() = 0 {}

    protected:
        virtual void awake() {}

        virtual void start() {}

        virtual void update() {}

        virtual void fixedUpdate() {}

        virtual void lateUpdate() {}

        virtual void onEnabled() {}

        virtual void onDisabled() {}

        bool isEnabled() const { return mEnabled; }

    private:
        void awakeInternal();

        void updateInternal();

        void fixedUpdateInternal();

        void lateUpdateInternal();

        void onEnabledInternal();

        void onDisabledInternal();

        bool mStarted = false;

        bool mEnabled = false;
    };
}

#endif

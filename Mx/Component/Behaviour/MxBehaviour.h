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

        virtual void onEnable() {}

        virtual void onDisable() {}

        bool isEnabled() const { return mEnabled; }

    private:
        void awakeInternal();

        void updateInternal();

        void fixedUpdateInternal();

        void lateUpdateInternal();

        void onEnableInternal();

        void onDisableInternal();

        bool mStarted = false;

        bool mEnabled = false;
    };
}

#endif

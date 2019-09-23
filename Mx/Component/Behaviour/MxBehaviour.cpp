#include "MxBehaviour.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(Behaviour, Component);

    void Behaviour::awakeInternal() {
        awake();
        onEnabledInternal();
    }

    void Behaviour::updateInternal() {
        if (!mStarted) {
            start();
            mStarted = true;
        }
        else {
            update();
        }
    }

    void Behaviour::fixedUpdateInternal() {
        if (!mStarted) {
            start();
            mStarted = true;
        }
        else {
            fixedUpdate();
        }
    }

    void Behaviour::lateUpdateInternal() {
        if (!mStarted) {
            start();
            mStarted = true;
        }
        else {
            lateUpdate();
        }
    }

    void Behaviour::onEnabledInternal() {
        mEnabled = true;
        onEnabled();
    }

    void Behaviour::onDisabledInternal() {
        mEnabled=false;
        onDisabled();
    }


}

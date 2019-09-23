#pragma once

#ifndef MX_AUDIO_LISTENER_H_
#define MX_AUDIO_LISTENER_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Audio/MxAudio.hpp"
#include "../../Math/MxVector3.h"

namespace Mix {
    /** @note Each @code Scene @endcode should only have 1 @code AudioListener @endcode. */
    class AudioListener final : public Behaviour {
        MX_DECLARE_RTTI;
    public:
        AudioListener(const Audio::VelocityUpdateMode _mode = Audio::VelocityUpdateMode::AUTO)
            : mVelocityUpdateMode(_mode),
            mCore(nullptr),
            mLastPos(0),
            mUseFixedUpdate(false) {
        }

        AudioListener(const AudioListener&) = default;
        ~AudioListener() = default;

    private:
        const int listenerIdx = 0;
        Audio::VelocityUpdateMode mVelocityUpdateMode;
        FMOD::System* mCore;
        Vector3f mLastPos;
        bool mUseFixedUpdate;

        void start() override;
        void fixedUpdate() override;
        void lateUpdate() override;

        void updatePosAndVel(const Vector3f& _pos, const Vector3f& _vel);

    };
}

#endif

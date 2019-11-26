#pragma once

#ifndef MX_AUDIO_LISTENER_H_
#define MX_AUDIO_LISTENER_H_

#include "../Behaviour/MxBehaviour.h"

namespace FMOD {
    class System;
}

namespace Mix {
    /**
     *  @brief This class acts as a proxy for 3d sound listener since there is only 1 single listener.
     *  @ref If the number of listeners is set to more than 1, then panning and doppler are turned off.
     *  All sound effects will be mono.
     *  FMOD uses a 'closest sound to the listener' method to determine what should be heard in this case.
     */
    class AudioListener final : public Behaviour {
    MX_DECLARE_RTTI

    public:
        AudioListener() : mCore(nullptr) {}

        //AudioListener(const AudioListener&) = default;

        ~AudioListener() = default;

    private:
        const int mListenerIdx = 0; /**< only 1 listener */

        FMOD::System* mCore;

        void start() override;
        void lateUpdate() override;
    };
}

#endif

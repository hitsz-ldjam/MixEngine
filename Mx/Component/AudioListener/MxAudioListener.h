#pragma once

#ifndef MX_AUDIO_LISTENER_H_
#define MX_AUDIO_LISTENER_H_

#include "../Behaviour/MxBehaviour.h"

#include <fmod/fmod.hpp>

namespace Mix {

    /**
     *  @note There should NOT be more than @code FMOD_MAX_LISTENERS @endcode listeners.\n
     *  If the number of listeners is more than 1, panning and doppler will be turned off, 
     *  and all sound effects will be mono. It's strongly recommended that there be one 
     *  singular @code AudioListener @endcode each @code Scene @endcode .
     */
    class AudioListener final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioListener();
        ~AudioListener() { --sListenerNum; }
        void update() override;

    private:
        static int sListenerNum;
        FMOD::System* mCore;
        int mListenerIdx;
    };
}

#endif

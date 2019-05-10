#pragma once

#ifndef MX_AUDIO_MANAGER_H
#define MX_AUDIO_MANAGER_H

#include <fmod/fmod.hpp>

#include "../../Behaviour/MxBehaviour.h"
#include "../../Exceptions/MxExceptions.hpp"

// Component
// | -- AudioSource(AudioClip)
// | -- AudioListener

namespace Mix {
    class AudioManager : public Behaviour {
    public:
        AudioManager() : mSystem(nullptr) {}
        ~AudioManager();
        void init() override;
        void update() override;

    private:
        FMOD::System* mSystem;
    };
}

#endif

#pragma once

#ifndef _MX_AUDIO_MANAGER_H_
#define _MX_AUDIO_MANAGER_H_

#include <string>
#include <stdexcept>

#include <fmod/fmod_studio.hpp>

namespace Mix {
    class AudioManager {
        friend class Application;
    public:
        FMOD::Studio::System* getSystem() { return system; }
        FMOD::System* getLowLevelSystem() { return lowLevelSystem; }
        ~AudioManager();

        static AudioManager& getInstance() {
            static AudioManager instance;
            return instance;
        }
        AudioManager(AudioManager const&) = delete;
        void operator=(AudioManager const&) = delete;

    private:
        FMOD::Studio::System* system = nullptr;
        FMOD::System* lowLevelSystem = nullptr;
        AudioManager();
        void update() { if(system) system->update(); }
    };
}

#endif

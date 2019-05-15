#pragma once

#ifndef MX_AUDIO_MANAGER_H_
#define MX_AUDIO_MANAGER_H_

#include <fmod/fmod_studio.hpp>

namespace Mix {
    class AudioManager {
        friend class Application;
    public:
        FMOD::Studio::System* GetSystem() const;
        FMOD::System* GetLowLevelSystem() const { return lowLevelSystem; }
        ~AudioManager();

        static AudioManager& GetInstance() {
            static AudioManager instance;
            return instance;
        }
        AudioManager(AudioManager const&) = delete;
        void operator=(AudioManager const&) = delete;

    private:
        FMOD::Studio::System* system = nullptr;
        FMOD::System* lowLevelSystem = nullptr;
        AudioManager();
        void Update() const { if(system) system->update(); }
    };
}

#endif

#include "MxAudioManager.h"

#include <iostream>

namespace Mix {
    AudioManager::AudioManager() {
        FMOD_RESULT result = FMOD::Studio::System::create(&system);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to create FMOD Studio");

        result = system->initialize(1024,
                                    FMOD_STUDIO_INIT_NORMAL,
                                    FMOD_INIT_NORMAL,
                                    nullptr);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to initialize FMOD Studio");

        result = system->getLowLevelSystem(&lowLevelSystem);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to initialize low level system");
    }

    FMOD::Studio::System* AudioManager::GetSystem() const { return system; }

    AudioManager::~AudioManager() {
        if(system)
            system->release();
        system = nullptr;
        lowLevelSystem = nullptr;
    }
}

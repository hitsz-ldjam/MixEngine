#include "MxAudioManager.h"

namespace Mix {
    AudioManager::~AudioManager() {
        if(mSystem)
            mSystem->release();
        mSystem = nullptr;
    }

    void AudioManager::init() {
        FMOD_RESULT result = FMOD::System_Create(&mSystem);
        if(result != FMOD_OK)
            throw FMODInitializationError();

        result = mSystem->init(1024, FMOD_INIT_NORMAL, nullptr);
        if(result != FMOD_OK)
            throw FMODInitializationError();
    }

    void AudioManager::update() {
        if(mSystem)
            mSystem->update();
    }
}

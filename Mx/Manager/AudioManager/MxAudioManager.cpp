#include "MxAudioManager.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioManager, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioManager)

    AudioManager::~AudioManager() {
        if(mCore)
            mCore->release();
        mCore = nullptr;
    }

    void AudioManager::init() {
        if(mCore)
            return;

        FMOD_RESULT result = FMOD::System_Create(&mCore);
        if(result != FMOD_OK)
            throw FmodInitializationError();

        result = mCore->init(512, FMOD_INIT_NORMAL, nullptr);
        if(result != FMOD_OK)
            throw FmodInitializationError();
    }

    void AudioManager::update() {
        if(mCore)
            mCore->update();
    }
}

#include "MxAudioClip.h"
#include "../../MixEngine.h"

namespace Mix {
    AudioClip::~AudioClip() {
        if(mSound)
            mSound->release();
        mSound = nullptr;
    }

    void AudioClip::loadAudioData() {
        mCore = MixEngine::Instance().getModule<Audio::Core>()->getCore();

        if(mSound) {
            mSound->release();
            mSound = nullptr;
        }

        auto loadMode = static_cast<FMOD_MODE>(loadType);
        loadMode |= FMOD_LOOP_NORMAL | FMOD_3D | FMOD_ACCURATETIME;
        if(loadInBackground)
            loadMode |= FMOD_NONBLOCKING;

        mCore->createSound(mPathStr.c_str(), loadMode, nullptr, &mSound);
    }

    Audio::LoadState AudioClip::loadState() const {
        if(!mSound)
            return Audio::LoadState::FAILED;

        FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
        auto result = mSound->getOpenState(&state, nullptr, nullptr, nullptr);

        if(result != FMOD_OK)
            return Audio::LoadState::FAILED;

        switch(state) {
            case FMOD_OPENSTATE_LOADING:
                return Audio::LoadState::LOADING;

            case FMOD_OPENSTATE_READY:
            case FMOD_OPENSTATE_PLAYING:
                return Audio::LoadState::LOADED;

            case FMOD_OPENSTATE_ERROR:
                return Audio::LoadState::FAILED;

            default:
                return Audio::LoadState::FAILED;
        }
    }

    float AudioClip::length() const {
        if(!mSound)
            return .0f;
        unsigned len = 0;
        if(mSound->getLength(&len, FMOD_TIMEUNIT_MS) != FMOD_OK)
            return .0f;
        return len / 1000.0f;
    }
}

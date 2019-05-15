#include "MxAudioClip.h"

namespace Mix {
    AudioClip::~AudioClip() {
        if(mSound)
            mSound->release();
        mSound = nullptr;
    }

    Audio::LoadState AudioClip::loadState(unsigned* _percentBuffered) const {
        if(!mSound)
            return Audio::LoadState::FAILED;
        FMOD_OPENSTATE state;
        mSound->getOpenState(&state, _percentBuffered, nullptr, nullptr);
        switch(state) {
            case FMOD_OPENSTATE_LOADING:
            case FMOD_OPENSTATE_CONNECTING:
            case FMOD_OPENSTATE_BUFFERING:
            case FMOD_OPENSTATE_SEEKING:
            case FMOD_OPENSTATE_SETPOSITION:
                return Audio::LoadState::LOADING;

            case FMOD_OPENSTATE_READY:
            case FMOD_OPENSTATE_PLAYING:
                return Audio::LoadState::LOADED;

            case FMOD_OPENSTATE_ERROR:
                return Audio::LoadState::FAILED;

            default:
                return Audio::LoadState::OTHER;
        }
    }

    float AudioClip::length() const {
        if(!mSound)
            return .0f;
        unsigned len;
        mSound->getLength(&len, FMOD_TIMEUNIT_MS);
        return len / 1000.0f;
    }

    FMOD_RESULT AudioClip::loadAudioData(const bool _loop, const bool _3D) {
        if(!mCore)
            mCore = Audio::Core();

        if(mSound) {
            auto result = mSound->release();
            if(result != FMOD_OK)
                return result;
            mSound = nullptr;
        }

        auto loadMode = static_cast<FMOD_MODE>(loadType);
        if(_loop)
            loadMode |= FMOD_LOOP_NORMAL;
        if(_3D)
            loadMode |= FMOD_3D;
        if(loadInBackground)
            loadMode |= FMOD_NONBLOCKING;

        return mCore->createSound(mPath.c_str(), loadMode, nullptr, &mSound);
    }

    // Obsolete
    //void AudioClip::loadFileToMemory(const char* _name, char** _buffer, long* _length) {
    //    FILE* stream = nullptr;
    //    stream = fopen(_name, "rb");
    //    fseek(stream, 0, SEEK_END);
    //    long length = ftell(stream);
    //    fseek(stream, 0, SEEK_SET);
    //    char* buffer = static_cast<char*>(malloc(length));
    //    fread(buffer, 1, length, stream);
    //    fclose(stream);
    //    *_buffer = buffer;
    //    *_length = length;
    //}
}

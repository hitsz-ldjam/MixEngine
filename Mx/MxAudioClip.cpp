#include "MxAudioClip.h"

namespace Mix {
    FMOD::Studio::System* AudioClip::System = AudioManager::GetInstance().GetSystem();
    FMOD::System* AudioClip::LowLevelSystem = AudioManager::GetInstance().GetLowLevelSystem();

    AudioClip::AudioClip(const std::string& _name, const AudioClipLoadType _type) {
        FMOD_RESULT result = LowLevelSystem->createSound(_name.c_str(), static_cast<unsigned>(_type), nullptr, &mSound);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to load audio " + _name);
    }

    AudioClip::~AudioClip() {
        if(mSound)
            mSound->release();
        mSound = nullptr;
    }

    FMOD::Channel* AudioClip::init(const bool _playOnInit, FMOD::ChannelGroup* _channelGroup) {
        if(getLoadState() == FMOD_OPENSTATE_READY && !mChannel)
            LowLevelSystem->playSound(mSound, _channelGroup, !_playOnInit, &mChannel);
        return mChannel;
    }

    FMOD_OPENSTATE AudioClip::getLoadState() {
        FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
        if(mSound)
            mSound->getOpenState(&state, nullptr, nullptr, nullptr);
        return state;
    }

    bool AudioClip::getMute() {
        bool mute = false;
        if(mChannel)
            mChannel->getMute(&mute);
        return mute;
    }

    bool AudioClip::getPaused() {
        bool paused = false;
        if(mChannel)
            mChannel->getPaused(&paused);
        return paused;
    }

    float AudioClip::getVolume() {
        float volume = 0.;
        if(mChannel)
            mChannel->getVolume(&volume);
        return volume;
    }

    void AudioClip::setVolume(const float _volume) const { if (mChannel) mChannel->setVolume(_volume); }

    void AudioClip::set3DAttributes(const glm::vec3* _position, const glm::vec3* _velocity) {
        FMOD_VECTOR pos, vel, *param1 = nullptr, *param2 = nullptr;
        if(_position) {
            pos = {_position->x, _position->y, _position->z};
            param1 = &pos;
        }
        if(_velocity) {
            vel = {_velocity->x, _velocity->y, _velocity->z};
            param2 = &vel;
        }
        if(mChannel)
            mChannel->set3DAttributes(param1, param2);
    }
}

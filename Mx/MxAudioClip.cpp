#include "MxAudioClip.h"

namespace Mix {
    FMOD::Studio::System* AudioClip::system = AudioManager::GetInstance().GetSystem();
    FMOD::System* AudioClip::lowLevelSystem = AudioManager::GetInstance().GetLowLevelSystem();

    AudioClip::AudioClip(const std::string& _name, const AudioClipLoadType _type) {
        FMOD_RESULT result = lowLevelSystem->createSound(_name.c_str(), static_cast<unsigned>(_type), nullptr, &sound);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to load audio " + _name);
    }

    AudioClip::~AudioClip() {
        if(sound)
            sound->release();
        sound = nullptr;
    }

    FMOD::Channel* AudioClip::Init(const bool _playOnInit, FMOD::ChannelGroup* _channelGroup) {
        if(GetLoadState() == FMOD_OPENSTATE_READY && !channel)
            lowLevelSystem->playSound(sound, _channelGroup, !_playOnInit, &channel);
        return channel;
    }

    FMOD_OPENSTATE AudioClip::GetLoadState() {
        FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
        if(sound)
            sound->getOpenState(&state, nullptr, nullptr, nullptr);
        return state;
    }

    bool AudioClip::GetMute() {
        bool mute = false;
        if(channel)
            channel->getMute(&mute);
        return mute;
    }

    bool AudioClip::GetPaused() {
        bool paused = false;
        if(channel)
            channel->getPaused(&paused);
        return paused;
    }

    float AudioClip::GetVolume() {
        float volume = 0.;
        if(channel)
            channel->getVolume(&volume);
        return volume;
    }

    void AudioClip::SetVolume(const float _volume) const { if (channel) channel->setVolume(_volume); }

    void AudioClip::Set3DAttributes(const glm::vec3* _position, const glm::vec3* _velocity) {
        FMOD_VECTOR pos, vel, *param1 = nullptr, *param2 = nullptr;
        if(_position) {
            pos = {_position->x, _position->y, _position->z};
            param1 = &pos;
        }
        if(_velocity) {
            vel = {_velocity->x, _velocity->y, _velocity->z};
            param2 = &vel;
        }
        if(channel)
            channel->set3DAttributes(param1, param2);
    }
}

#include "MxAudioClip.h"

namespace Mix {
    FMOD::Studio::System* AudioClip::system = AudioManager::getInstance().getSystem();
    FMOD::System* AudioClip::lowLevelSystem = AudioManager::getInstance().getLowLevelSystem();

    AudioClip::AudioClip(const std::string& name, const AudioClipLoadType type) {
        FMOD_RESULT result = lowLevelSystem->createSound(name.c_str(), static_cast<unsigned>(type), nullptr, &sound);
        if(result != FMOD_OK)
            throw std::runtime_error("Error: Failed to load audio " + name);
    }

    AudioClip::~AudioClip() {
        if(sound)
            sound->release();
        sound = nullptr;
    }

    FMOD::Channel* AudioClip::init(const bool playOnInit, FMOD::ChannelGroup* channelGroup) {
        if(getLoadState() == FMOD_OPENSTATE_READY && !channel)
            lowLevelSystem->playSound(sound, channelGroup, !playOnInit, &channel);
        return channel;
    }

    FMOD_OPENSTATE AudioClip::getLoadState() {
        FMOD_OPENSTATE state = FMOD_OPENSTATE_ERROR;
        if(sound)
            sound->getOpenState(&state, nullptr, nullptr, nullptr);
        return state;
    }

    bool AudioClip::getMute() {
        bool mute = false;
        if(channel)
            channel->getMute(&mute);
        return mute;
    }

    bool AudioClip::getPaused() {
        bool paused = false;
        if(channel)
            channel->getPaused(&paused);
        return paused;
    }

    float AudioClip::getVolume() {
        float volume = 0.;
        if(channel)
            channel->getVolume(&volume);
        return volume;
    }

    void AudioClip::set3DAttributes(const glm::vec3* position, const glm::vec3* velocity) {
        FMOD_VECTOR pos, vel, *param1 = nullptr, *param2 = nullptr;
        if(position) {
            pos = {position->x, position->y, position->z};
            param1 = &pos;
        }
        if(velocity) {
            vel = {velocity->x, velocity->y, velocity->z};
            param2 = &vel;
        }
        if(channel)
            channel->set3DAttributes(param1, param2);
    }
}

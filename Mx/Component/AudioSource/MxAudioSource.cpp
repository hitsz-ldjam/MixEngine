#include "MxAudioSource.h"

#include "../../GameObject/MxGameObject.h"
#include "../RigidBody/MxRigidBody.h"
#include "../../Time/MxTime.h"

#include <fmod/fmod.hpp>

namespace Mix {
    MX_IMPLEMENT_RTTI(AudioSource, Component);


    void AudioSource::setMute(const bool _mute) {
        if (mChannel)
            mChannel->setMute(_mute);
        else
            mChannelParam.mute = _mute;
    }

    bool AudioSource::getMute() const {
        if (mChannel)
            mChannel->getMute(&mChannelParam.mute);
        return mChannelParam.mute;
    }

    bool AudioSource::getPaused() const {
        if (mChannel)
            mChannel->getPaused(&mChannelParam.paused);
        return mChannelParam.paused;
    }

    void AudioSource::setPaused(const bool _paused) {
        if (mChannel)
            mChannel->setPaused(_paused);
        else
            mChannelParam.paused = _paused;
    }

    float AudioSource::getVolume() const {
        if (mChannel)
            mChannel->getVolume(&mChannelParam.volume);
        return mChannelParam.volume;
    }

    void AudioSource::setVolume(const float _volume) {
        if (mChannel)
            mChannel->setVolume(_volume);
        else
            mChannelParam.volume = _volume;
    }

    bool AudioSource::getVolumeRamp() const {
        if (mChannel)
            mChannel->getVolumeRamp(&mChannelParam.volumRamp);
        return mChannelParam.volumRamp;
    }

    void AudioSource::setVolumeRamp(const bool _volumeRamp) {
        if (mChannel)
            mChannel->setVolumeRamp(_volumeRamp);
        else
            mChannelParam.volumRamp = _volumeRamp;
    }

    float AudioSource::getPitch() const {
        if (mChannel)
            mChannel->getPitch(&mChannelParam.pitch);
        return mChannelParam.pitch;
    }

    void AudioSource::setPitch(const float _pitch) {
        if (mChannel)
            mChannel->setPitch(_pitch);
        else
            mChannelParam.pitch = _pitch;
    }

    Vector2f AudioSource::get3DMinMaxDistance() const {
        if (mChannel)
            mChannel->get3DMinMaxDistance(&mChannelParam.distance.x, &mChannelParam.distance.y);
        return mChannelParam.distance;
    }

    void AudioSource::set3DMinMaxDistance(const Vector2f& _distance) {
        if (mChannel)
            mChannel->set3DMinMaxDistance(_distance.x, _distance.y);
        else
            mChannelParam.distance = _distance;
    }

    float AudioSource::get3DDopplerLevel() const {
        if (mChannel)
            mChannel->get3DDopplerLevel(&mChannelParam.level);
        return mChannelParam.level;
    }

    void AudioSource::set3DDopplerLevel(const float _level) {
        if (mChannel)
            mChannel->set3DDopplerLevel(_level);
        else
            mChannelParam.level = _level;
    }

    float AudioSource::getFrequency() const {
        if (mChannel)
            mChannel->getFrequency(&mChannelParam.Frequency);
        return mChannelParam.Frequency;
    }

    void AudioSource::setFrequency(const float _frequency) {
        if (mChannel)
            mChannel->setFrequency(_frequency);
        else
            mChannelParam.Frequency = _frequency;
    }

    int AudioSource::getPriority() const {
        if (mChannel)
            mChannel->getPriority(&mChannelParam.Priority);
        return mChannelParam.Priority;
    }

    void AudioSource::setPriority(const int _priority) {
        if (mChannel)
            mChannel->setPriority(_priority);
        else
            mChannelParam.Priority = _priority;
    }


    void FMODChannelParam::importChannel(FMOD::Channel& _Channel) {
        _Channel.getMute(&mute);
        _Channel.getPaused(&paused);
        _Channel.getVolume(&volume);
        _Channel.getVolumeRamp(&volumRamp);
        _Channel.getPitch(&pitch);
        _Channel.get3DMinMaxDistance(&distance.x, &distance.y);
        _Channel.get3DLevel(&level);
        _Channel.getFrequency(&Frequency);
        _Channel.getPriority(&Priority);
    }

    void FMODChannelParam::exportChannel(FMOD::Channel& _Channel) {
        _Channel.setMute(mute);
        _Channel.setPaused(paused);
        _Channel.setVolume(volume);
        _Channel.setVolumeRamp(volumRamp);
        _Channel.setPitch(pitch);
        _Channel.set3DMinMaxDistance(distance.x, distance.y);
        _Channel.set3DLevel(level);
        _Channel.setFrequency(Frequency);
        _Channel.setPriority(Priority);
    }
}

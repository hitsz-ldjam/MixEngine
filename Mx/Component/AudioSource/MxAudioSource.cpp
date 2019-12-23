#include "MxAudioSource.h"

#include "../../Audio/MxAudioCore.h"
#include "../../Resource/Audio/MxAudioClip.h"

#include "../Transform/MxTransform.h"
#include "../RigidBody/MxRigidBody.h"
#include "../../GameObject/MxGameObject.h"

#include <fmod/fmod.hpp>

namespace Mix {
    MX_IMPLEMENT_RTTI(AudioSource, Behaviour);

    AudioSource::AudioSource(std::shared_ptr<AudioClip> _clip, const bool _playOnAwake, const bool _loop)
        : mClip(std::move(_clip)),
          //                              DO NOT use delete
          mChannel(nullptr, [](auto* _) { if(_) _->stop(); }),
          mAddiParam(_playOnAwake, _loop) {}

    void AudioSource::setClip(std::shared_ptr<AudioClip> _clip) {
        mChannelParam.importFrom(mChannel.get());

        mClip = std::move(_clip);
        loadClip();

        startNewChannel();
    }

    bool AudioSource::getLoop() const {
        if(mChannel) {
            FMOD_MODE mode = 0;
            mChannel->getMode(&mode);
            mAddiParam.loop = !(mode & FMOD_LOOP_OFF); // FMOD_LOOP_OFF overrides FMOD_LOOP_NORMAL
        }
        return mAddiParam.loop;
    }

    void AudioSource::setLoop(const bool _loop) {
        if(mChannel)
            mChannel->setMode(_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        mAddiParam.loop = _loop;
    }

    void AudioSource::play() {
        if(mChannel) {
            mChannel->setPosition(0, FMOD_TIMEUNIT_MS);
            mChannel->setPaused(false);
        }
    }

    void AudioSource::pause() {
        if(mChannel)
            mChannel->setPaused(true);
    }

    void AudioSource::resume() {
        if(mChannel)
            mChannel->setPaused(false);
    }

    bool AudioSource::isPlaying() const {
        bool p = false;
        if(mChannel)
            mChannel->isPlaying(&p);
        return p;
    }

    bool AudioSource::isVirtual() const {
        bool v = true; // todo: default to true?
        if(mChannel)
            mChannel->isVirtual(&v);
        return v;
    }

    float AudioSource::getPitch() const {
        if(mChannel)
            mChannel->getPitch(&mChannelParam.pitch);
        return mChannelParam.pitch;
    }

    void AudioSource::setPitch(const float _pitch) {
        if(mChannel)
            mChannel->setPitch(_pitch);
        mChannelParam.pitch = _pitch;
    }

    float AudioSource::getFrequency() const {
        if(mChannel)
            mChannel->getFrequency(&mChannelParam.frequency);
        return mChannelParam.frequency;
    }

    void AudioSource::setFrequency(const float _frequency) {
        if(mChannel)
            mChannel->setFrequency(_frequency);
        mChannelParam.frequency = _frequency;
    }

    int AudioSource::getPriority() const {
        if(mChannel)
            mChannel->getPriority(&mChannelParam.priority);
        return mChannelParam.priority;
    }

    void AudioSource::setPriority(const int _priority) {
        if(mChannel)
            mChannel->setPriority(_priority);
        mChannelParam.priority = _priority;
    }

    double AudioSource::getPosition() const {
        unsigned position = 0;
        if(mChannel)
            mChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
        return position / 1000.0;
    }

    void AudioSource::setPosition(const double _position) {
        if(mChannel)
            mChannel->setPosition(_position * 1000, FMOD_TIMEUNIT_MS);
    }

    float AudioSource::getVolume() const {
        if(mChannel)
            mChannel->getVolume(&mChannelParam.volume);
        return mChannelParam.volume;
    }

    void AudioSource::setVolume(const float _volume) {
        if(mChannel)
            mChannel->setVolume(_volume);
        mChannelParam.volume = _volume;
    }

    bool AudioSource::getVolumeRamp() const {
        if(mChannel)
            mChannel->getVolumeRamp(&mChannelParam.volumRamp);
        return mChannelParam.volumRamp;
    }

    void AudioSource::setVolumeRamp(const bool _volumeRamp) {
        if(mChannel)
            mChannel->setVolumeRamp(_volumeRamp);
        mChannelParam.volumRamp = _volumeRamp;
    }

    bool AudioSource::getMute() const {
        if(mChannel)
            mChannel->getMute(&mChannelParam.mute);
        return mChannelParam.mute;
    }

    void AudioSource::setMute(const bool _mute) {
        if(mChannel)
            mChannel->setMute(_mute);
        mChannelParam.mute = _mute;
    }

    Vector2f AudioSource::get3DMinMaxDistance() const {
        if(mChannel)
            mChannel->get3DMinMaxDistance(&mChannelParam.minMaxDistance.x, &mChannelParam.minMaxDistance.y);
        return mChannelParam.minMaxDistance;
    }

    void AudioSource::set3DMinMaxDistance(const Vector2f& _distance) {
        if(mChannel)
            mChannel->set3DMinMaxDistance(_distance.x, _distance.y);
        mChannelParam.minMaxDistance = _distance;
    }

    float AudioSource::get3DDopplerLevel() const {
        if(mChannel)
            mChannel->get3DDopplerLevel(&mChannelParam.dopplerLevel);
        return mChannelParam.dopplerLevel;
    }

    void AudioSource::set3DDopplerLevel(const float _level) {
        if(mChannel)
            mChannel->set3DDopplerLevel(_level);
        mChannelParam.dopplerLevel = _level;
    }

    void AudioSource::awake() {
        loadClip();
        startNewChannel();
    }

    void AudioSource::lateUpdate() {
        if(!mChannel)
            return;
        Vector3f pos = transform()->getPosition(), vel(0);
        if(auto rb = mGameObject->getComponent<RigidBody>())
            vel = rb->getLinearVelocity();
        mChannel->set3DAttributes(reinterpret_cast<FMOD_VECTOR*>(pos.linear),
                                  reinterpret_cast<FMOD_VECTOR*>(vel.linear));
    }

    void AudioSource::loadClip() {
        FMOD::Channel* channel = nullptr;
        if(mClip)
            Audio::Core::Get()->getCore()->playSound(mClip->getSound(), nullptr, true, &channel);
        mChannel.reset(channel);
    }

    void AudioSource::startNewChannel() {
        mChannelParam.exportTo(mChannel.get());
        setLoop(mAddiParam.loop);
        if(mAddiParam.playOnAwake)
            resume();
    }
}

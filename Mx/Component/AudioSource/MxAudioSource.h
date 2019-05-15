#pragma once

#ifndef MX_AUDIO_SOURCE_H_
#define MX_AUDIO_SOURCE_H_

#include "../Behaviour/MxBehaviour.h"
#include "MxAudioClip.h"

// todo: Coordinate with Rigidbody

namespace Mix {
    /*class Rigidbody;*/

    // todo: 3d roll off
    class AudioSource final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioSource(AudioClip* _clip = nullptr) : playOnAwake(true),
                                                  velocityUpdateMode(Audio::VelocityUpdateMode::AUTO),
                                                  mClip(_clip),
                                                  mChannel(nullptr),
                                                  mLastPos(0),
                                                  mUseFixedUpdate(false) {}

        ~AudioSource() = default;

        // ----- Properties -----

        bool playOnAwake;

        Audio::VelocityUpdateMode velocityUpdateMode;

        const AudioClip* clip() const { return mClip; }

        void clip(AudioClip* _clip) { mClip = _clip; }

        float dopplerLevel() const {
            float dl = 1.0f;
            if(mChannel)
                mChannel->get3DDopplerLevel(&dl);
            return dl;
        }

        /**
         *  @breif Determines how much doppler effect will be applied to this audio source.
         *  @param _dopplerLevel Doppler scale from 0.0 (none), to 1.0 (normal) to 5.0 (exaggerated), default = 1.0.
         */
        void dopplerLevel(const float _dopplerLevel) {
            if(mChannel)
                mChannel->set3DDopplerLevel(_dopplerLevel);
        }

        bool isPlaying() const {
            bool ip = false;
            if(mChannel)
                mChannel->isPlaying(&ip);
            return ip;
        }

        bool isVirtual() const {
            bool iv = true;
            if(mChannel)
                mChannel->isVirtual(&iv);
            return iv;
        }

        // todo: fix execution order issue
        bool loop() const {
            if(mChannel) {
                FMOD_MODE m;
                mChannel->getMode(&m);
                return m & FMOD_LOOP_NORMAL;
            }
            return false;
        }

        void loop(const bool _loop) {
            if(mChannel)
                mChannel->setMode(_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
        }

        /**
         *  @note This is a getter.
         */
        bool mute() const {
            bool m = false;
            if(mChannel)
                mChannel->getMute(&m);
            return m;
        }

        void mute(const bool _mute) {
            if(mChannel)
                mChannel->setMute(_mute);
        }

        /**
         *  @param _pan Pan level, from -1.0 (left) to 1.0 (right), default = 0 (center).
         *  @note AKA Stereo Pan. There is NO getter for this property.
         */
        void pan(const float _pan) {
            if(mChannel)
                mChannel->setPan(_pan);
        }

        float pitch() const {
            float pitch = 0.0f;
            if(mChannel)
                mChannel->getPitch(&pitch);
            return pitch;
        }

        /**
         *  @brief Scales existing frequency values by the pitch.
         *  @param _pitch Pitch value, 0.5 = half pitch, 2.0 = double pitch, etc default = 1.0.
         */
        void pitch(const float _pitch) {
            if(mChannel)
                mChannel->setPitch(_pitch);
        }

        int priority() const {
            int priority = 128;
            if(mChannel)
                mChannel->getPriority(&priority);
            return priority;
        }

        /**
         *  @param _priority An integer between 0 and 255. 0 = highest priority, 255 = lowest priority.
         */
        void priority(const int _priority) {
            if(mChannel)
                mChannel->setPriority(_priority);
        }

        float time() const {
            unsigned t = 0;
            if(mChannel)
                mChannel->getPosition(&t, FMOD_TIMEUNIT_MS);
            return t / 1000.0f;
        }

        void time(const float _time) {
            if(mChannel)
                mChannel->setPosition(static_cast<unsigned>(_time * 1000), FMOD_TIMEUNIT_MS);
        }

        float volume() const {
            float volume = 1.0f;
            if(mChannel)
                mChannel->getVolume(&volume);
            return volume;
        }

        /**
         *  @param _volume Linear volume level. Recommended be in range [0, 1].
         *  @note Volume level can be below 0 to invert a signal and above 1 to amplify the signal. 
         *  Increasing the signal level too far may cause audible distortion.
         */
        void volume(const float _volume) {
            if(mChannel)
                mChannel->setVolume(_volume);
        }

        // ----- Stream controlling -----

        void play();

        void pause() {
            if(mChannel)
                mChannel->setPaused(true);
        }

        void unpause() {
            if(mChannel)
                mChannel->setPaused(false);
        }

        void stop() {
            if(mChannel)
                mChannel->stop();
        }

    private:
        AudioClip* mClip;
        FMOD::Channel* mChannel;
        glm::vec3 mLastPos;
        bool mUseFixedUpdate;

        void init() override;
        void lateUpdate() override;
        void fixedUpdate() override;

        void updatePosAndVel(const float _deltaTime);
    };
}

#endif

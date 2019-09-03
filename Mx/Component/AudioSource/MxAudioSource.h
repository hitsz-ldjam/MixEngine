#pragma once

#ifndef MX_AUDIO_SOURCE_H_
#define MX_AUDIO_SOURCE_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Audio/MxAudioClip.h"
#include "../../Math/MxVector3.h"

namespace Mix {
    // todo: add 3d roll off
    class AudioSource final : public Behaviour {
        MX_DECLARE_RTTI
            MX_DECLARE_CLASS_FACTORY

    public:
        /** @note Default ctor is for RTTI. DO NOT use this ctor. */
        AudioSource() : mPlayOnAwake(true),
            mVelocityUpdateMode(Audio::VelocityUpdateMode::AUTO),
            mClip(nullptr),
            mChannel(nullptr),
            mLastPos(0),
            mUseFixedUpdate(false),
            mDopplerLevel(1.0f),
            mLoop(false),
            mMute(false),
            mPan(0.0f),
            mPitch(1.0f),
            mPriority(128),
            mVolume(1.0f) {
        }

        AudioSource(AudioClip* _clip,
                    const bool _playOnAwake = true,
                    const Audio::VelocityUpdateMode _mode = Audio::VelocityUpdateMode::AUTO)
            : mPlayOnAwake(_playOnAwake),
            mVelocityUpdateMode(_mode),
            mClip(_clip),
            mChannel(nullptr),
            mLastPos(0),
            mUseFixedUpdate(false),
            mDopplerLevel(1.0f),
            mLoop(false),
            mMute(false),
            mPan(0.0f),
            mPitch(1.0f),
            mPriority(128),
            mVolume(1.0f) {
        }

/** @note Copy constructed AudioSources are initially stopped. */
        AudioSource(const AudioSource& _other) : Behaviour(),
            mPlayOnAwake(_other.mPlayOnAwake),
            mVelocityUpdateMode(_other.mVelocityUpdateMode),
            mClip(_other.mClip),
            mChannel(nullptr),
            mLastPos(_other.mLastPos),
            mUseFixedUpdate(_other.mUseFixedUpdate),
            mDopplerLevel(_other.mDopplerLevel),
            mLoop(_other.mLoop),
            mMute(_other.mMute),
            mPan(_other.mPan),
            mPitch(_other.mPitch),
            mPriority(_other.mPriority),
            mVolume(_other.mVolume) {
        }

        ~AudioSource() { stop(); }

        // ----- Properties -----

        const AudioClip* clip() const { return mClip; }

        void clip(AudioClip* _clip) { mClip = _clip; }

        float dopplerLevel() const { return mDopplerLevel; }

        /**
         *  @brief Determines how much doppler effect will be applied to this audio source.
         *  @param _dopplerLevel Doppler scale from 0.0 (none), to 1.0 (normal) to 5.0 (exaggerated), default = 1.0.
         */
        void dopplerLevel(const float _dopplerLevel);

        bool isPlaying() const;

        bool isVirtual() const;

        bool loop() const { return mLoop; }

        void loop(const bool _loop);

        /**
         *  @note This is a getter. To mute the clip, consider @code AudioSource::mute(true) @endcode.
         */
        bool mute() const { return mMute; }

        void mute(const bool _mute);

        /**
         *  @return The pan level set by user. This value may differ from the real one.
         */
        float pan() const { return mPan; }

        /**
         *  @brief Pan level affects the left right balance of the sound.
         *  @param _pan Pan level, from -1.0 (left) to 1.0 (right), default = 0 (center).
         *  @note AKA Stereo Pan.
         */
        void pan(const float _pan);

        float pitch() const { return mPitch; }

        /**
         *  @brief Scales existing frequency values by the pitch.
         *  @param _pitch Pitch value, 0.5 = half pitch, 2.0 = double pitch, etc. Default = 1.0.
         */
        void pitch(const float _pitch);

        int priority() const { return mPriority; }

        /**
         *  @param _priority An integer between 0 and 255. 0 = highest priority, 255 = lowest priority.
         *  Default = 128.
         */
        void priority(const int _priority);

        float volume() const { return mVolume; }

        /**
         *  @param _volume Linear volume level. Recommended be in range [0, 1].
         *  @note Volume level can be below 0 to invert a signal and above 1 to amplify the signal.
         *  Increasing the signal level too far may cause audible distortion.
         */
        void volume(const float _volume);

        // ----- Stream controlling -----

        /**
         *  @note This function creates a new channel to play the clip.
         *  Nothing would happen if the clip is not ready.
         *  @see AudioSource::unpause()
         */
        void play();

        void pause() {
            if (mChannel)
                mChannel->setPaused(true);
        }

        /**
         *  @brief Unpause the clip; no channel created.
         */
        void unpause() {
            if (mChannel)
                mChannel->setPaused(false);
        }

        void stop() {
            if (mChannel)
                mChannel->stop();
        }

        float time() const {
            unsigned t = 0;
            if (mChannel)
                mChannel->getPosition(&t, FMOD_TIMEUNIT_MS);
            return t / 1000.0f;
        }

        /**
         *  @brief Set the playback position in seconds.
         */
        void time(const float _time) {
            if (mChannel)
                mChannel->setPosition(static_cast<unsigned>(_time * 1000), FMOD_TIMEUNIT_MS);
        }

    private:
        bool mPlayOnAwake;
        Audio::VelocityUpdateMode mVelocityUpdateMode;

        AudioClip* mClip;
        FMOD::Channel* mChannel;

        Math::Vector3f mLastPos;
        bool mUseFixedUpdate;

        // use FMOD::ChannelGroup instead?
        float mDopplerLevel;
        bool mLoop;
        bool mMute;
        float mPan;
        float mPitch;
        int mPriority;
        float mVolume;

        void init() override;
        void fixedUpdate() override;
        void lateUpdate() override;

        void initChannelParameters();
        void updatePosAndVel(const Math::Vector3f& _pos, const Math::Vector3f& _vel);
    };
}

#endif

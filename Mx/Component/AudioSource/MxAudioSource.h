#pragma once

#ifndef MX_AUDIO_SOURCE_H_
#define MX_AUDIO_SOURCE_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Audio/MxAudioUtils.h"

namespace FMOD {
    class Channel;
}

namespace Mix {
    class AudioClip;

    class AudioSource final : public Behaviour {
    MX_DECLARE_RTTI

    public:
        AudioSource(std::shared_ptr<AudioClip> _clip,
                    const bool _playOnAwake = true,
                    const bool _loop = false);

        ~AudioSource() = default;

        // AudioSource Config

        auto getClip() const { return mClip; }
        /**
         *  @note Due to implementing issues, only a part of params are reserved for the new AudioSource.
         *  See@code Audio::FMODChannelParam@endcode for further details.
         */
        void setClip(std::shared_ptr<AudioClip> _clip);

        /** @return The low-level fmod channel of this AudioSource. */
        auto getFmodChannel() const { return mChannel.get(); }

        bool getPlayOnAwake() const { return mAddiParam.playOnAwake; }
        void setPlayOnAwake(const bool _playOnAwake) { mAddiParam.playOnAwake = _playOnAwake; }

        bool getLoop() const;
        void setLoop(const bool _loop);

        // Playback Control & Info

        /** @brief Plays the audio from beginning. */
        void play();
        void pause();
        void resume();
        bool isPlaying() const;
        bool isVirtual() const;

        float getPitch() const;
        /**
         *  @brief Scales existing frequency values by the pitch.
         *  @param _pitch 0.5 represents half pitch (one octave down),
         *  1.0 represents unmodified pitch and 2.0 represents double pitch (one octave up).
         */
        void setPitch(const float _pitch);

        /**
         *  @return Frequency / playback rate. Unit: Hertz.
         */
        float getFrequency() const;
        void setFrequency(const float _frequency);

        int getPriority() const;
        /**
         *  @param _priority Less priority value means more important. Range: [0, 256], default: 128.
         */
        void setPriority(const int _priority);

        double getPosition() const;
        /**
         *  @param _position Playback position. Unit: second.
         */
        void setPosition(const double _position);

        // Volume Levels

        float getVolume() const;
        /**
         *  @brief Set volume level. Negative level inverts the signal.
         *  @param _volume Range: (-inf, +inf) Default: 1
         */
        void setVolume(const float _volume);

        bool getVolumeRamp() const;
        /** @param _volumeRamp If true, volume changes are ramped. Otherwise instantaneous. */
        void setVolumeRamp(const bool _volumeRamp);

        bool getMute() const;
        void setMute(const bool _mute);

        // Spatial Simulation

        // todo: 3DCustomRolloff

        /**
         *  @brief Retrieves the minimum and maximum distances used to calculate the 3D roll off attenuation.
         *  @return A vector whose x component indicates the min distance and the other indicates the max distance.
         *  @note FMOD_3D_CUSTOMROLLOFF overrides the settings here. (a to-do)
         */
        Vector2f get3DMinMaxDistance() const;
        /**
         *  @param _distance x component: range: (0, y), default: 1. y component: range: (x, inf), default: 10000
         */
        void set3DMinMaxDistance(const Vector2f& _distance);

        float get3DDopplerLevel() const;
        /**
         *  @param _level Doppler scale. Range: [0, 5], default: 1. 0 represents no doppler, 1 represents natural doppler and 5 represents exaggerated doppler.
         */
        void set3DDopplerLevel(const float _level);

    private:
        std::shared_ptr<AudioClip> mClip;
        std::unique_ptr<FMOD::Channel, std::function<void(FMOD::Channel*)>> mChannel;
        mutable Audio::FMODChannelParam mChannelParam;

        mutable struct AdditionalParam {
            bool playOnAwake;
            bool loop;

            explicit AdditionalParam(const bool _playOnAwake,
                                     const bool _loop) : playOnAwake(_playOnAwake),
                                                         loop(_loop) {}
        } mAddiParam;

        void awake() override;
        void lateUpdate() override;

        void loadClip();
        void startNewChannel();
    };
}

#endif

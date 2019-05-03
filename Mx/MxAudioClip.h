#pragma once

#ifndef _MX_AUDIO_CLIP_H_
#define _MX_AUDIO_CLIP_H_

#include <glm/vec3.hpp>

#include <string>
#include "MxAudioManager.h"

namespace Mix {
    enum class AudioClipLoadType : unsigned {
        Loop = FMOD_LOOP_NORMAL,
        ThreeD = FMOD_3D,
        Streaming = FMOD_CREATESTREAM,
        DecompressOnLoad = FMOD_CREATESAMPLE,
        CompressedInMemory = FMOD_CREATECOMPRESSEDSAMPLE,
        LoadInBackground = FMOD_NONBLOCKING,
    };

    inline AudioClipLoadType operator& (AudioClipLoadType a, AudioClipLoadType b) {
        return static_cast<AudioClipLoadType>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
    }

    inline AudioClipLoadType operator| (AudioClipLoadType a, AudioClipLoadType b) {
        return static_cast<AudioClipLoadType>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }
    
    class AudioClip {
    public:
        static AudioClip* Open(const std::string& _filename,
                               const AudioClipLoadType _type = AudioClipLoadType::DecompressOnLoad) {
            return new AudioClip(_filename, _type);
        };

        explicit AudioClip(const std::string& _name, const AudioClipLoadType _type = AudioClipLoadType::DecompressOnLoad);
        ~AudioClip();
        /** @return The channel that the audio is played on. */
        FMOD::Channel* Init(const bool _playOnInit = true, FMOD::ChannelGroup* _channelGroup = nullptr);

        FMOD_OPENSTATE GetLoadState();
        bool GetMute();
        bool GetPaused();
        float GetVolume();
        void Pause() const { if(channel) channel->setPaused(true); }
        void Play() const { if(channel) channel->setPaused(false); }
        void SetMute(const bool _mute = true) const { if(channel) channel->setMute(_mute); }
        /**
         * @param _volume Linear volume level. Can be below 0 to invert a signal and above 1 to amplify the signal.
         * @note Increasing the signal level too far may cause audible distortion.
         */
        void SetVolume(const float _volume) const;
        void Stop() const { if(channel) channel->stop(); }

        FMOD::Sound* GetSound() const { return sound; }
        FMOD::Channel* GetChannel() const { return channel; }

        // todo: add more interfaces

        // todo: implement with Transform
        // get3DAttributes
        /**
         * @param _position Pass nullptr to ignore.
         * @param _velocity Pass nullptr to ignore.
         */
        void Set3DAttributes(const glm::vec3* _position = nullptr, const glm::vec3* _velocity = nullptr);

    private:
        static FMOD::Studio::System* system;
        static FMOD::System* lowLevelSystem;
        FMOD::Sound* sound = nullptr;
        FMOD::Channel* channel = nullptr;
    };
}

#endif

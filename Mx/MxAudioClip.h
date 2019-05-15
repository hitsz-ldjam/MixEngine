#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

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

    inline AudioClipLoadType operator& (AudioClipLoadType _a, AudioClipLoadType _b) {
        return static_cast<AudioClipLoadType>(static_cast<unsigned>(_a) & static_cast<unsigned>(_b));
    }

    inline AudioClipLoadType operator| (AudioClipLoadType _a, AudioClipLoadType _b) {
        return static_cast<AudioClipLoadType>(static_cast<unsigned>(_a) | static_cast<unsigned>(_b));
    }
    
    class AudioClip {
    public:
        static AudioClip* Open(const std::string& _filename,
                               const AudioClipLoadType _type = AudioClipLoadType::DecompressOnLoad) {
            return new AudioClip(_filename, _type);
        }

        explicit AudioClip(const std::string& _name, const AudioClipLoadType _type = AudioClipLoadType::DecompressOnLoad);
        ~AudioClip();
        /** @return The channel that the audio is played on. */
        FMOD::Channel* init(const bool _playOnInit = true, FMOD::ChannelGroup* _channelGroup = nullptr);

        FMOD_OPENSTATE getLoadState();
        bool getMute();
        bool getPaused();
        float getVolume();
        void pause() const { if(mChannel) mChannel->setPaused(true); }
        void play() const { if(mChannel) mChannel->setPaused(false); }
        void setMute(const bool _mute = true) const { if(mChannel) mChannel->setMute(_mute); }
        /**
         * @param _volume Linear volume level. Can be below 0 to invert a signal and above 1 to amplify the signal.
         * @note Increasing the signal level too far may cause audible distortion.
         */
        void setVolume(const float _volume) const;
        void stop() const { if(mChannel) mChannel->stop(); }

        FMOD::Sound* getSound() const { return mSound; }
        FMOD::Channel* getChannel() const { return mChannel; }

        // todo: add more interfaces

        // todo: implement with Transform
        // get3DAttributes
        /**
         * @param _position Pass nullptr to ignore.
         * @param _velocity Pass nullptr to ignore.
         */
        void set3DAttributes(const glm::vec3* _position = nullptr, const glm::vec3* _velocity = nullptr);

    private:
        static FMOD::Studio::System* System;
        static FMOD::System* LowLevelSystem;
        FMOD::Sound* mSound = nullptr;
        FMOD::Channel* mChannel = nullptr;
    };
}

#endif

#pragma once

#ifndef _MX_AUDIO_CLIP_H_
#define _MX_AUDIO_CLIP_H_

#include <glm/vec3.hpp>

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
        static AudioClip* open(const std::string& filename,
                               const AudioClipLoadType type = AudioClipLoadType::DecompressOnLoad) {
            return new AudioClip(filename, type);
        };

        AudioClip(const std::string& name, const AudioClipLoadType type = AudioClipLoadType::DecompressOnLoad);
        ~AudioClip();
        /** @return The channel that the audio is played on. */
        FMOD::Channel* init(const bool playOnInit = true, FMOD::ChannelGroup* channelGroup = nullptr);

        FMOD_OPENSTATE getLoadState();
        bool getMute();
        bool getPaused();
        float getVolume();
        void pause() { if(channel) channel->setPaused(true); }
        void play() { if(channel) channel->setPaused(false); }
        void setMute(const bool mute = true) { if(channel) channel->setMute(mute); }
        /**
         * @param volume Linear volume level. Can be below 0 to invert a signal and above 1 to amplify the signal.
         * @note Increasing the signal level too far may cause audible distortion.
         */
        void setVolume(const float volume) { if(channel) channel->setVolume(volume); }
        void stop() { if(channel) channel->stop(); }

        FMOD::Sound* getSound() { return sound; }
        FMOD::Channel* getChannel() { return channel; }

        // todo: add more interfaces

        // todo: implement with Transform
        // get3DAttributes
        /**
         * @param position Pass nullptr to ignore.
         * @param velocity Pass nullptr to ignore.
         */
        void set3DAttributes(const glm::vec3* position = nullptr, const glm::vec3* velocity = nullptr);

    private:
        static FMOD::Studio::System* system;
        static FMOD::System* lowLevelSystem;
        FMOD::Sound* sound = nullptr;
        FMOD::Channel* channel = nullptr;
    };
}

#endif

#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

#include "MxAudio.hpp"
#include <filesystem>

namespace Mix {
    class AudioClip final {
        friend class AudioSource;

    public:
        AudioClip(const std::filesystem::path& _path,
                  const Audio::LoadType& _loadType = Audio::LoadType::DECOMPRESS_ON_LOAD,
                  const bool _preloadAudioData = true)
            : preloadAudioData(_preloadAudioData),
              loadInBackground(false),
              loadType(_loadType),
              mCore(nullptr),
              mSound(nullptr),
              mPathStr(_path.string()) {
            // todo: load this in AssetManager (if we would ever have one)
            if(preloadAudioData)
                loadAudioData();
        }

        AudioClip(const AudioClip&) = delete;

        ~AudioClip();

        bool preloadAudioData;    /**< Default to true. */
        bool loadInBackground;    /**< Default to false. */
        Audio::LoadType loadType; /**< Default to @code Audio::LoadType::DECOMPRESS_ON_LOAD @endcode */

        /**
         *  @node Suggest using @code AudioSource::loadState() @endcode to check load state 
         *  before further operations especially when the clip is loaded in background.
         */
        void loadAudioData();

        Audio::LoadState loadState() const;

        float length() const;

    private:
        FMOD::System* mCore;
        FMOD::Sound* mSound;
        std::string mPathStr;
    };
}

#endif

#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

//#define _CRT_SECURE_NO_WARNINGS

#include "../../Definitions/MxAudio.h"

#include <filesystem>

namespace Mix {
    class AudioClip final {
        friend class AudioSource;

    public:
        /**
         *  @note Default constructor is only for Rtti. Do NOT use this manually.
         */
        AudioClip() : mCore(nullptr),
                      mSound(nullptr) {}

        AudioClip(const std::filesystem::path& _path,
                  const Audio::LoadType& _loadType = Audio::LoadType::DECOMPRESS_ON_LOAD)
            : loadInBackground(false),
              loadType(_loadType),
              mCore(nullptr),
              mSound(nullptr),
              mPath(_path.string()) {}

        ~AudioClip();

        const bool preloadAudioData = true; /**< Forced to true. */
        bool loadInBackground;              /**< Default is false. */
        Audio::LoadType loadType;           /**< Default is @code Audio::LoadType::DECOMPRESS_ON_LOAD @endcode */

        Audio::LoadState loadState(unsigned* _percentBuffered = nullptr) const;

        float length() const;

    private:
        FMOD::System* mCore;
        FMOD::Sound* mSound;
        std::string mPath;

        FMOD_RESULT loadAudioData(const bool _loop = false, const bool _3D = true);

        // Obsolete
        //void loadFileToMemory(const char* _name, char** _buffer, long* _length);
    };
}

#endif

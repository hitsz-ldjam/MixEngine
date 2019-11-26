#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

#include <fmod/fmod_common.h>
#include <string>

namespace FMOD {
    class Sound;
}

namespace Mix {
    namespace Audio {
        enum class LoadMode {
            Streaming = FMOD_CREATESTREAM,
            /**< Decompress at run time and stream from the source. */

            DecompressOnLoad = FMOD_CREATESAMPLE,
            /**< Decompress at load time and decode whole file into memory. Fastest for playback and most flexible. */

            CompressedInMemory = FMOD_CREATECOMPRESSEDSAMPLE,
            /**< Load MP2 / MP3 / FADPCM / IMAADPCM / Vorbis / AT9 or XMA into memory and leave it compressed. */
        };
    }

    // todo: make it a resource
    class AudioClip final {
    public:
        AudioClip(std::string _path) : mPath(std::move(_path)) {}
        ~AudioClip() = default;
        void load();
        double getLength() const;
        auto getSound() const noexcept { return mSound.get(); }

    private:
        static std::shared_ptr<FMOD::Sound> LoadAudio(const std::string& _path,
                                                      Audio::LoadMode _mode = Audio::LoadMode::DecompressOnLoad);

        std::string mPath;
        std::shared_ptr<FMOD::Sound> mSound;
    };
}

#endif

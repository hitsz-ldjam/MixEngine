// OBSOLETE

#pragma once

#ifndef _MX_AUDIO_H_
#define _MX_AUDIO_H_

#include <stdexcept>
#include <string>

#include <SDL2/SDL_mixer.h>

namespace Mix {
    class MxAudio {
    public:
        enum MxAudioType { MX_AUDIO_TYPE_MUSIC, MX_AUDIO_TYPE_SOUNDFX };
        MxAudio(const MxAudioType type);
        ~MxAudio();
        void load(const std::string filename);
        // negative value for infinite loop
        void play(const int playTimes = 1);
        void pause();
        void resume();
    private:
        Mix_Music* music;
        Mix_Chunk* soundFX;
        MxAudioType type;
    };
}

#endif

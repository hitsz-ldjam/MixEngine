#include "MXAudio.h"

namespace Mixel {
    MXAudio::MXAudio(const MXAudio::AudioType type) {
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                music = nullptr;
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                soundFX = nullptr;
                break;
            default:
                throw std::runtime_error("Error: Unknown audio type");
                break;
        }
        this->type = type;
    }

    MXAudio::~MXAudio() {
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                if(music) {
                    Mix_FreeMusic(music);
                    music = nullptr;
                }
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                if(soundFX) {
                    Mix_FreeChunk(soundFX);
                    soundFX = nullptr;
                }
                break;
        }
    }

    void MXAudio::load(const std::string filename) {
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                music = Mix_LoadMUS(filename.c_str());
                if(!music) {
                    throw std::runtime_error("Error: Failed to open file " + filename);
                }
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                soundFX = Mix_LoadWAV(filename.c_str());
                if(!soundFX) {
                    throw std::runtime_error("Error: Failed to open file " + filename);
                }
                break;
        }
    }

    void MXAudio::play(const int playTimes) {
        if(playTimes == 0) return;
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                if(!music) {
                    throw std::runtime_error("Error: Music is null");
                }
                Mix_PlayMusic(music, playTimes < 0 ? -1 : playTimes);
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                if(!soundFX) {
                    throw std::runtime_error("Error: Sound FX is null");
                }
                Mix_PlayChannel(-1, soundFX, playTimes < 0 ? -1 : playTimes - 1);
                break;
        }
    }

    void MXAudio::pause() {
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                if(!music) {
                    throw std::runtime_error("Error: Music is null");
                }
                // Todo: create implementation
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                if(!soundFX) {
                    throw std::runtime_error("Error: Sound FX is null");
                }
                // Todo: create implementation
                break;
        }
    }

    void MXAudio::resume() {
        switch(type) {
            case MX_AUDIO_TYPE_MUSIC:
                if(!music) {
                    throw std::runtime_error("Error: Music is null");
                }
                // Todo: create implementation
                break;
            case MX_AUDIO_TYPE_SOUNDFX:
                if(!soundFX) {
                    throw std::runtime_error("Error: Sound FX is null");
                }
                // Todo: create implementation
                break;
        }
    }
}

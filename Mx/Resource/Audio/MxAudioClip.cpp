#include "MxAudioClip.h"
#include "../../Audio/MxAudioCore.h"
#include <fmod/fmod.hpp>

namespace Mix {
    void AudioClip::load() {
        mSound = LoadAudio(mPath);
    }

    double AudioClip::getLength() const {
        if(!mSound)
            return .0;
        unsigned len = 0;
        mSound->getLength(&len, FMOD_TIMEUNIT_MS);
        return len / 1000.0;
    }

    std::shared_ptr<FMOD::Sound> AudioClip::LoadAudio(const std::string& _path, Audio::LoadMode _mode) {
        FMOD::Sound* sound = nullptr;
        auto core = Audio::Core::Get()->getCore();
        core->createSound(_path.c_str(),
                          static_cast<FMOD_MODE>(_mode) | FMOD_LOOP_NORMAL | FMOD_3D | FMOD_ACCURATETIME | FMOD_LOWMEM,
                          nullptr,
                          &sound);
        //                                                          DO NOT use delete
        return std::shared_ptr<FMOD::Sound>(sound, [](auto* _) { if(_) _->release(); });
    }
}

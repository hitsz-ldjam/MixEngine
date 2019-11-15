#include "MxAudioCore.h"
#include "../../MixEngine.h"
#include <fmod/fmod.hpp>

namespace Mix::Audio {
    Core::~Core() {
        if(mCore)
            // DO NOT use delete
            mCore->release();
        mCore = nullptr;
    }

    Core* Core::Get() {
        return MixEngine::Instance().getModule<Core>();
    }

    void Core::load() {
        auto result = FMOD::System_Create(&mCore);
        if(result != FMOD_OK) MX_EXCEPT("Failed to initialize FMOD")
        result = mCore->init(maxChannels, FMOD_INIT_NORMAL, nullptr);
        if(result != FMOD_OK) MX_EXCEPT("Failed to initialize FMOD")
    }

    void Core::update() { mCore->update(); }
}

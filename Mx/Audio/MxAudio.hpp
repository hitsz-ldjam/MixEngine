#pragma once

#ifndef MX_AUDIO_HPP_
#define MX_AUDIO_HPP_

#include "../Engine/MxModuleBase.h"
#include <fmod/fmod.hpp>
#include <glm/vec3.hpp>

namespace Mix::Audio {
    enum class LoadType {
        STREAMING = FMOD_CREATESTREAM,
        DECOMPRESS_ON_LOAD = FMOD_CREATESAMPLE,
        COMPRESSED_IN_MEMORY = FMOD_CREATECOMPRESSEDSAMPLE,
    };

    enum class LoadState {
        LOADING,
        LOADED,
        FAILED,
    };

    enum class VelocityUpdateMode {
        AUTO,
        FIXED,
        DYNAMIC
    };

    inline FMOD_VECTOR glmVec3ToFmodVec(const glm::vec3& _vec) {
        return {_vec.x, _vec.y, _vec.z};
    }

    class Core final : public ModuleBase {
    public:
        Core() : mCore(nullptr) {}
        ~Core() { if(mCore) mCore->release(); }

        auto getCore() const noexcept { return mCore; }

        void awake() {
            auto result = FMOD::System_Create(&mCore);
            if(result != FMOD_OK)
                throw ThirdPartyLibInitError("FMOD");
            result = mCore->init(512, FMOD_INIT_NORMAL, nullptr);
            if(result != FMOD_OK)
                throw ThirdPartyLibInitError("FMOD");
        }

        void update() {
            mCore->update();
        }

        // Auto called when ModuleHolder destructs?
        void release() {
            if(mCore)
                mCore->release();
            mCore = nullptr;
        }

    private:
        FMOD::System* mCore;
    };
}

#endif

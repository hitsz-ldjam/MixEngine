#pragma once

#ifndef MX_AUDIO_DEFINITIONS_H_
#define MX_AUDIO_DEFINITIONS_H_

#include <fmod/fmod.hpp>
#include <glm/vec3.hpp>

namespace Mix {
    namespace Audio {
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

        FMOD::System* Core();
    }
}

#endif

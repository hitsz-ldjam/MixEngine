#pragma once

#ifndef MX_AUDIO_MANAGER_H_
#define MX_AUDIO_MANAGER_H_

#include "../../Component/Behaviour/MxBehaviour.h"

#include <fmod/fmod.hpp>
#include <glm/vec3.hpp>

namespace Mix {
    namespace Audio {
        inline FMOD_VECTOR glmVec3ToFmodVec(const glm::vec3& _vec) {
            return {_vec.x, _vec.y, _vec.z};
        }
    }

    class AudioManager final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioManager() : mCore(nullptr) {}
        ~AudioManager();
        void init() override;
        void update() override;

        FMOD::System* core() const { return mCore; }

    private:
        FMOD::System* mCore;
    };
}

#endif

#pragma once

#ifndef MX_AUDIO_SOURCE_H_
#define MX_AUDIO_SOURCE_H_

#include "../Behaviour/MxBehaviour.h"

namespace Mix {
    // todo
    class AudioSource final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioSource() = default;
        ~AudioSource() = default;
    };
}

#endif

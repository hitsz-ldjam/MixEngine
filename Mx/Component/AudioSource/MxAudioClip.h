#pragma once

#ifndef MX_AUDIO_CLIP_H_
#define MX_AUDIO_CLIP_H_

#include "../../Object/MxObject.h"

namespace Mix {
    // todo
    class AudioClip final : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        AudioClip() = default;
        ~AudioClip() = default;
    };
}

#endif

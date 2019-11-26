#pragma once

#ifndef MX_AUDIO_UTILS_H_
#define MX_AUDIO_UTILS_H_

#include "../Math/MxVector2.h"

namespace FMOD {
    class Channel;
}

namespace Mix::Audio {
    struct FMODChannelParam {
        float pitch;
        float frequency;
        int priority;
        float volume;
        bool volumRamp;
        bool mute;
        Vector2f minMaxDistance;
        float dopplerLevel;

        explicit FMODChannelParam() : pitch(1),
                                      frequency(0),
                                      priority(0),
                                      volume(1),
                                      volumRamp(false),
                                      mute(false),
                                      minMaxDistance(1, 10000),
                                      dopplerLevel(1) {}

        void importFrom(FMOD::Channel* _channel);
        void exportTo(FMOD::Channel* _channel);
    };
}

#endif

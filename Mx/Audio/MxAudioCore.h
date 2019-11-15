#pragma once

#ifndef MX_AUDIO_CORE_H_
#define MX_AUDIO_CORE_H_

#include "../Engine/MxModuleBase.h"

namespace FMOD {
    class System;
}

namespace Mix::Audio {
    class Core final : public ModuleBase {
    public:
        Core() : mCore(nullptr) {}

        ~Core();

        static Core* Get();

        auto getCore() const noexcept { return mCore; }

        void load() override;

        void init() override {}

        /** @note Call this in lateUpdate() */
        void update();

        const int maxChannels = 512; /**< range: [0, 4093] */

    private:
        FMOD::System* mCore;
    };
}

#endif

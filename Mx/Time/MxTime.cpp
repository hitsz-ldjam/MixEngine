#include "MxTime.h"
#include <algorithm>
#include <cmath>

namespace Mix {
    float Time::sDeltaTime = 0.0f,
        Time::sTime = 0.0f,
        Time::sFixedDeltaTime = 1.0f / 60,
        Time::sFixedTime = 0.0f,
        Time::sMaximumDeltaTime = 1.0f / 3,
        Time::sSmoothingFactor = 0.0f;

    unsigned Time::sFixedClampedSteps = 0;

    Time::TimePoint Time::sStart = Clock::now(),
        Time::sPrev = Clock::now(),
        Time::sCurr = Clock::now();

    float Time::RealTime() noexcept {
        return DurationToSecond(Clock::now() - sStart);
    }

    void Time::Awake() noexcept {
        sDeltaTime = sTime = sFixedTime = sSmoothingFactor = 0.0f;
        sFixedDeltaTime = 1.0f / 60;
        sMaximumDeltaTime = 1.0f / 3;
        sFixedClampedSteps = 0;
        sStart = sPrev = sCurr = Clock::now();
    }

    void Time::Tick() noexcept {
        sCurr = Clock::now();
        sDeltaTime = DurationToSecond(sCurr - sPrev);
        sTime = DurationToSecond(sCurr - sStart);

        const int steps = static_cast<int>(std::floor((sTime - sFixedTime) / sFixedDeltaTime));
        const int maxSteps = static_cast<int>(std::floor((sMaximumDeltaTime - sDeltaTime) / sFixedDeltaTime));
        // in case delta time is already larger than max delta time
        sFixedClampedSteps = std::max(std::min(steps, maxSteps), 0);

        // avoid rounding issues
        if (steps)
            sFixedTime += steps * sFixedDeltaTime;

        sSmoothingFactor = (sTime - sFixedTime) / sFixedDeltaTime;

        sPrev = sCurr;
    }
}

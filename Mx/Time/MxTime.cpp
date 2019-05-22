#include "MxTime.h"

#include <algorithm>

namespace Mix {
    float Time::sDeltaTime = 0.0f,
          Time::sTime = 0.0f,
          Time::sFixedDeltaTime = 1.0f / 60,
          Time::sFixedTime = 0.0f,
          Time::sMaximumDeltaTime = 1.0f / 3;

    Time::TimePoint Time::sStart = Time::Clock::now(),
                    Time::sPrev = Time::Clock::now(),
                    Time::sCurr = Time::Clock::now();

    float Time::sSmoothing = 0.0f;
    int Time::sFixedClampedSteps = 0;

    void Time::Tick() {
        sCurr = Clock::now();
        sDeltaTime = DurationToSecond(sCurr - sPrev);
        sTime = DurationToSecond(sCurr - sStart);

        const int steps = static_cast<int>(std::floor((sTime - sFixedTime) / sFixedDeltaTime));
        // avoid rounding issues
        if(steps)
            sFixedTime += steps * sFixedDeltaTime;

        sSmoothing = (sTime - sFixedTime) / sFixedDeltaTime;

        const int maxSteps = static_cast<int>(std::floor((sMaximumDeltaTime - sDeltaTime) / sFixedDeltaTime));
        // in case delta time is already larger than max delta time
        sFixedClampedSteps = std::max(std::min(steps, maxSteps), 0);

        sPrev = sCurr;
    }
}

#pragma once
#ifndef _MX_TIMER_H_
#define _MX_TIMER_H_


#include "MxDef.h"
#include "MxObject.h"
#include <chrono>


namespace Mix {
    class Timer {
    public:
        double totalTime();

        double deltaTime() const {
            return mDeltaTime;
        }

        void reset();
        void start();
        void stop();
        void tick();

    private:
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
        using Duration = std::chrono::steady_clock::duration;
        using Clock = std::chrono::steady_clock;

        double mDeltaTime = 0.0;

        bool mPaused = true;

        TimePoint mStartTp;
        TimePoint mStopTp;
        TimePoint mPrevTp;
        TimePoint mCurrTp;
        Duration mPausedTime;

        double durToSeconds(const Duration& dur) {
            return static_cast<double>(dur.count()) * Duration::period::num / Duration::period::den;
        }
    };
}
#endif // !_MX_TIMER_H_

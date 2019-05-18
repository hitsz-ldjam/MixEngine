#pragma once
#ifndef _MX_TIMER_H_
#define _MX_TIMER_H_


#include <chrono>


namespace Mix {
    class Timer {
    public:
        double TotalTime();

        double DeltaTime() const {
            return mDeltaTime;
        }

        void Reset();
        void Start();
        void Stop();
        void Tick();

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
        Duration mPausedTime = std::chrono::steady_clock::duration(0);

        static double DurToSeconds(const Duration& _dur) {
            return static_cast<double>(_dur.count()) * Duration::period::num / Duration::period::den;
        }
    };
}
#endif // !_MX_TIMER_H_

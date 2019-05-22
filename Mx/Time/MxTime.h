#pragma once

#ifndef MX_TIME_H_
#define MX_TIME_H_

#include <chrono>

namespace Mix {
    class Time final {
        friend class MixEngine;

    public:
        Time() = delete;

        /**
         *  @note Caution: At the very moment this class is reset, delta time equals 0.
         */
        static float DeltaTime() { return sDeltaTime; }

        /** @note Equivalent to Time.time of Unity5. */
        static float TotalTime() { return sTime; }

        /** @note Bullet3 uses an internal fixed time step of 60 Hertz. Thus the default value is 1/60. */
        static float FixedDeltaTime() { return sFixedDeltaTime; }

        static void FixedDeltaTime(const float _fixedDeltaTime) { sFixedDeltaTime = _fixedDeltaTime; }

        static float FixedTime() { return sFixedTime; }

        static float MaximumDeltaTime() { return sMaximumDeltaTime; }

        /**
         *  @brief Set the maximum time of a frame. If a frame takes longer than it, fixedUpdate() 
         *  will not be performed.
         *  @param _maximumDeltaTime The maximum time a frame can take.
         *  It is advisable to keep maximum time for frame between 1/10 and 1/3 of a second.
         */
        static void MaximumDeltaTime(const float _maximumDeltaTime) { sMaximumDeltaTime = _maximumDeltaTime; }

    private:
        using Clock = std::chrono::steady_clock;
        using Duration = Clock::duration;
        using TimePoint = std::chrono::time_point<Clock>;

        static float sDeltaTime,
                     sTime,
                     sFixedDeltaTime,
                     sFixedTime,
                     sMaximumDeltaTime;

        static TimePoint sStart, sPrev, sCurr;

        static float sSmoothing;
        static int sFixedClampedSteps;

        static void Init() {
            sDeltaTime = 0.0f;
            sTime = 0.0f;
            sFixedDeltaTime = 1.0f / 60;
            sFixedTime = 0.0f;
            sMaximumDeltaTime = 1.0f / 3;
            sFixedClampedSteps = 0;
            sStart = sPrev = sCurr = Clock::now();
            sSmoothing = 0.0f;
            sFixedClampedSteps = 0;
        }

        static void Tick();

        static float DurationToSecond(const Duration& _duration) {
            return static_cast<float>(_duration.count()) * Duration::period::num / Duration::period::den;
        }
    };
}

#endif

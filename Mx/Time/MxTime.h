#pragma once

#ifndef MX_TIME_H_
#define MX_TIME_H_

#include <chrono>

namespace Mix {
    class Time final {
        friend class MixEngine;

    public:
        Time() = delete;

        Time(const Time&) = delete;

        /** @note CAUTION! At the very moment this class is reset, delta time equals 0. */
        static auto DeltaTime() noexcept { return sDeltaTime; }

        /** @note Equivalent to Time.time of Unity5. */
        static auto TotalTime() noexcept { return sTime; }

        /** @note Bullet3 uses an internal fixed time step of 60 Hertz. Thus the default value is 1/60. */
        static auto& FixedDeltaTime() noexcept { return sFixedDeltaTime; }

        static auto FixedTime() noexcept { return sFixedTime; }

        /**
         *  @brief Set the maximum time of a frame. If a frame takes longer than it, fixedUpdate()
         *  will not be performed.
         *  It is advisable to keep maximum time for frame between 1/10 and 1/3 of a second.
         */
        static auto& MaximumDeltaTime() noexcept { return sMaximumDeltaTime; }

        static auto SmoothingFactor() noexcept { return sSmoothingFactor; }

        static auto FixedClampedSteps() noexcept { return sFixedClampedSteps; }

    private:
        using Clock = std::chrono::steady_clock;
        using Duration = Clock::duration;
        using TimePoint = std::chrono::time_point<Clock>;

        static float sDeltaTime,
                     sTime,
                     sFixedDeltaTime,
                     sFixedTime,
                     sMaximumDeltaTime,
                     sSmoothingFactor;

        static unsigned sFixedClampedSteps;

        static TimePoint sStart, sPrev, sCurr;

        static void Awake() noexcept;

        static void Tick() noexcept;

        static auto DurationToSecond(const Duration& _duration) noexcept {
            return static_cast<float>(_duration.count()) * Duration::period::num / Duration::period::den;
        }
    };
}

#endif

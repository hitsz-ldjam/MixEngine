#pragma once

#ifndef _MX_TIME_H_
#define _MX_TIME_H_

#include <chrono>

namespace Mix {
    class Time {
        friend class Application;
    public:
        static double getTime() { return time; };
        static double getDeltaTime() { return deltaTime; };
		static double totalTime();

		static void reset();
		static void start();
		static void stop();
		static void tick();
    private:
		using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
		using Duration = std::chrono::steady_clock::duration;
		using Clock = std::chrono::steady_clock;

		static double mDeltaTime;
		static bool mPaused;

		static TimePoint mStartTp;
		static TimePoint mStopTp;
		static TimePoint mPrevTp;
		static TimePoint mCurrTp;
		static Duration mPausedTime;

        static double time;
        static double deltaTime;
        static double getDuration(const std::chrono::high_resolution_clock::time_point& startPoint) {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startPoint);
            return static_cast<double>(duration.count()) / 10e8;
        }

		static double durToSeconds(const Duration& dur) {
			return static_cast<double>(dur.count())* Duration::period::num / Duration::period::den;
		}
    };
}

#endif

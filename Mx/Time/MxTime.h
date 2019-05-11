#pragma once

#ifndef _MX_TIME_H_
#define _MX_TIME_H_

#include <chrono>

namespace Mix {
    /** @note deprecated */
    class Time {
        friend class MixEngine;
    public:
        static double TotalTime() { return mTime; };
        static double DeltaTime() { return mDeltaTime; };
    private:
        static double mTime;
        static double mDeltaTime;
        static double getDuration(const std::chrono::high_resolution_clock::time_point& startPoint) {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startPoint);
            return static_cast<double>(duration.count()) / 10e8;
        }
    };
}

#endif

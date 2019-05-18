#pragma once

#ifndef _MX_TIME_H_
#define _MX_TIME_H_

#include <chrono>

namespace Mix {
    class Time {
        friend class Application;
    public:
        static double GetTime() { return time; };
        static double GetDeltaTime() { return deltaTime; };
    private:
        static double time;
        static double deltaTime;
        static double GetDuration(const std::chrono::high_resolution_clock::time_point& _startPoint) {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - _startPoint);
            return static_cast<double>(duration.count()) / 10e8;
        }
    };
}

#endif

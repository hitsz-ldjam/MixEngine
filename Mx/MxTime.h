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
    private:
        static double time;
        static double deltaTime;
        static double getDuration(const std::chrono::high_resolution_clock::time_point& startPoint) {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startPoint);
            return static_cast<double>(duration.count()) / 10e8;
        }
    };
}

#endif
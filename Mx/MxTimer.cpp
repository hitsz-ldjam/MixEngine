#include "MxTimer.h"

namespace Mix {
    double Timer::totalTime() {
        if (mPaused)
            return durToSeconds(mStopTp - mStartTp - mPausedTime);
        else
            return durToSeconds(mCurrTp - mStartTp - mPausedTime);
    }

    void Timer::reset() {
        mStartTp = Clock::now();
        mPrevTp = mStartTp;
        mPaused = false;
    }

    void Timer::start() {
        if (mPaused) {
            auto now = Clock::now();
            mPausedTime += now - mStopTp;
            mPrevTp = now;
            mPaused = false;
        }
    }

    void Timer::stop() {
        if (!mPaused) {
            mStopTp = Clock::now();
            mPaused = true;
        }
    }

    void Timer::tick() {
        if (mPaused) {
            mDeltaTime = 0.0;
            return;
        }

        mCurrTp = Clock::now();
        mDeltaTime = durToSeconds(mCurrTp - mPrevTp);
        mPrevTp = mCurrTp;
    }

}
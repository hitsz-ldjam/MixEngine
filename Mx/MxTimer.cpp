#include "MxTimer.h"

namespace Mix {
    double Timer::TotalTime() {
        if (mPaused)
            return DurToSeconds(mStopTp - mStartTp - mPausedTime);
        else
            return DurToSeconds(mCurrTp - mStartTp - mPausedTime);
    }

    void Timer::Reset() {
        mStartTp = Clock::now();
        mPrevTp = mStartTp;
        mPaused = false;
    }

    void Timer::Start() {
        if (mPaused) {
            auto now = Clock::now();
            mPausedTime += now - mStopTp;
            mPrevTp = now;
            mPaused = false;
        }
    }

    void Timer::Stop() {
        if (!mPaused) {
            mStopTp = Clock::now();
            mPaused = true;
        }
    }

    void Timer::Tick() {
        if (mPaused) {
            mDeltaTime = 0.0;
            return;
        }

        mCurrTp = Clock::now();
        mDeltaTime = DurToSeconds(mCurrTp - mPrevTp);
        mPrevTp = mCurrTp;
    }

}
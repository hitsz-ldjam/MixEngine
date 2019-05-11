#include "MxTime.h"

namespace Mix {
    double Time::time = 0.;
    double Time::deltaTime = 0.;
	double Time::mDeltaTime = 0.0;
	double Time::totalTime() {
		if (mPaused)
			return durToSeconds(mStopTp - mStartTp - mPausedTime);
		else
			return durToSeconds(mCurrTp - mStartTp - mPausedTime);
	}

	void Time::reset() {
		mStartTp = Clock::now();
		mPrevTp = mStartTp;
		mPaused = false;
	}

	void Time::start() {
		if (mPaused) {
			auto now = Clock::now();
			mPausedTime += now - mStopTp;
			mPrevTp = now;
			mPaused = false;
		}
	}

	void Time::stop() {
		if (!mPaused) {
			mStopTp = Clock::now();
			mPaused = true;
		}
	}

	void Time::tick() {
		if (mPaused) {
			mDeltaTime = 0.0;
			return;
		}

		mCurrTp = Clock::now();
		mDeltaTime = durToSeconds(mCurrTp - mPrevTp);
		mPrevTp = mCurrTp;
	}
}

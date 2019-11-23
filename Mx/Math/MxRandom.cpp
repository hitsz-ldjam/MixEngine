#include "MxRandom.h"
#include "../Definitions/MxDefinitions.h"

namespace Mix {

    Random::Random(uint32 _seed) {
        setSeed(_seed);
    }

    void Random::setSeed(uint32 _seed) {
        mSeed[0] = _seed;
        mSeed[1] = _seed * 0x71f34a9b + 1;
        mSeed[2] = _seed * 0x42eacb2d + 1;
        mSeed[3] = _seed * 0x8d3fca9e + 1;
    }

    uint32 Random::getRaw() const {
        uint32 t = mSeed[3];
        t ^= t << 11;
        t ^= t >> 8;

        mSeed[3] = mSeed[2];
        mSeed[2] = mSeed[1];
        mSeed[1] = mSeed[0];

        const uint32 s = mSeed[0];
        t ^= s;
        t ^= s >> 19;

        mSeed[0] = t;
        return t;
    }

    int32 Random::getRange(int32 _min, int32 _max) const {
        MX_ASSERT(_min < _max);

        const int32 range = _max - _min + 1;

        return _min + int32(getReal()*range);
    }

    uint32 Random::getRange(uint32 _min, uint32 _max) const {
        MX_ASSERT(_min < _max);

        const uint32 range = _max - _min + 1;

        return _min + uint32(getReal()*(float(range) - 0.00001f));
    }

    float Random::getRange(float _min, float _max) const {
        return _min + getReal()*(_max - _min);
    }

    float Random::getReal() const {
        return float(getRaw() & 0x007FFFFF) / 8388607.0f;
    }
}

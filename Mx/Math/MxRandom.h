#pragma once
#ifndef MX_RANDOM_H_
#define MX_RANDOM_H_
#include "../Definitions/MxTypes.h"

namespace Mix {
    class Random {
    public:
        explicit Random(uint32 _seed = 0);

        void setSeed(uint32 _seed);

        /**
         * \brief Return a random value in range [0, std::numeric_limits<uint32_t>::max()] 
         */
        uint32 getRaw() const;

        /**
         * \brief Return a random value in range [0, 1] 
         */
        float getReal() const;

        /**
         * \brief Return a random value in range [_min, _max] 
         */
        int32 getRange(int32 _min, int32 _max) const;

        /**
         * \brief Return a random value in range [_min, _max] 
         */
        uint32 getRange(uint32 _min, uint32 _max) const;

        /**
         * \brief Return a random value in range [_min, _max] 
         */
        float getRange(float _min, float _max) const;

    private:
        mutable uint32 mSeed[4];
    };
}

#endif

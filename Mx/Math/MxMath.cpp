#include "MxMath.h"
#include "MxVector.h"

namespace Mix {
    namespace Math {
        uint16 FloatToHalf(float _f) {
            union { float f; uint32_t i; } v;
            v.f = _f;

            int32_t s = (v.i >> 16) & 0x00008000;
            int32_t e = ((v.i >> 23) & 0x000000ff) - (127 - 15);
            int32_t m = v.i & 0x007fffff;

            if (e <= 0) {
                if (e < -10) {
                    return 0;
                }
                m = (m | 0x00800000) >> (1 - e);

                return static_cast<uint16_t>(s | (m >> 13));
            }
            else if (e == 0xff - (127 - 15)) {
                if (m == 0) // Inf
                {
                    return static_cast<uint16_t>(s | 0x7c00);
                }
                else    // NAN
                {
                    m >>= 13;
                    return static_cast<uint16_t>(s | 0x7c00 | m | (m == 0));
                }
            }
            else {
                if (e > 30) // Overflow
                {
                    return static_cast<uint16_t>(s | 0x7c00);
                }

                return static_cast<uint16_t>(s | (e << 10) | (m >> 13));
            }
        }

        float HalfToFloat(uint16 _h) {
            int32_t s = (_h >> 15) & 0x00000001;
            int32_t e = (_h >> 10) & 0x0000001f;
            int32_t m = _h & 0x000003ff;

            if (e == 0) {
                if (m == 0) // Plus or minus zero
                {
                    return s << 31;
                }
                else // Denormalized number -- renormalize it
                {
                    while (!(m & 0x00000400)) {
                        m <<= 1;
                        e -= 1;
                    }

                    e += 1;
                    m &= ~0x00000400;
                }
            }
            else if (e == 31) {
                if (m == 0) // Inf
                {
                    return (s << 31) | 0x7f800000;
                }
                else // NaN
                {
                    return (s << 31) | 0x7f800000 | (m << 13);
                }
            }

            e = e + (127 - 15);
            m = m << 13;

            union { float f; uint32_t i; } v;
            v.i = (s << 31) | (e << 23) | m;
            return v.f;
        }
    }
}
#pragma once
#ifndef MX_MATH_H_
#define MX_MATH_H_
#include "../Definitions/MxGeneralBase.hpp"

namespace Mix {
    namespace Math {
        class Constants :public GeneralBase::StaticBase{
        public:
            constexpr static float Epsilon = 0.000001f;
            constexpr static float Pi = 3.141593f;
        };

        template<typename _Pre, typename _Lst>
        _Pre Align(_Pre const& _size, _Lst const& _alignment) {
            return (_size + _alignment - 1) & ~(_alignment - 1);
        }

        template<typename _Ty>
        _Ty LeastMultiple(_Ty const& _a, _Ty const& _b) {
            _Ty m = _a, n = _b, c;
            while (_b != 0) {
                c = _a % _b;
                _a = _b;
                _b = c;
            }
            return m * n / _a;
        }

        template<typename _Ty>
        _Ty NextPowerOf2(_Ty const& _size) {
            const auto power = static_cast<_Ty>(std::log2l(static_cast<long double>(_size))) + 1;
            return static_cast<_Ty>(1) << power;
        }

        template<typename _Ty>
        bool IsPowerOf2(_Ty const& _size) {
            _Ty mask = 0;
            const auto power = static_cast<uint32_t>(std::log2l(static_cast<long double>(_size)));

            for (uint32_t i = 0; i < power; ++i)
                mask += static_cast<_Ty>(1) << i;

            return !(_size & mask);
        }
    }
}

#endif

#pragma once
#ifndef MX_MATH_IMPL_H_
#define MX_MATH_IMPL_H_

#include "../Utils/MxGeneralBase.hpp"
#include <cmath>
#include "../Definitions/MxTypes.h"

namespace Mix {
    namespace Math {
        class Constants :public GeneralBase::StaticBase {
        public:
            constexpr static float Epsilon = 0.000001f;
            constexpr static float Pi = 3.1415926535f;
            constexpr static float Pi2 = Pi * 2;
        };

        template<typename _Pre, typename _Lst>
        static auto Align(_Pre const& _size, _Lst const& _alignment) {
            return (_size + _alignment - 1) & ~(_alignment - 1);
        }


        template<typename _Ty>
        static _Ty Clamp(_Ty const& _vaule, _Ty const& _min, _Ty const& _max) {
            return std::clamp(_vaule, _min, _max);
        }

        template<typename _Ty>
        static _Ty Wrap(_Ty const& _value, _Ty const& _min, _Ty const& _max) {
            auto dur = _max - _min;
            if (_value < _min)
                return _value + std::floor((_max - _value) / dur) * dur;
            else if (_value > _max)
                return _value - std::floor((_value - _min) / dur) * dur;
            else
                return _value;
        }

        template<typename _Ty>
        static _Ty LeastMultiple(_Ty const& _a, _Ty const& _b) {
            _Ty m = _a, n = _b, c;
            while (_b != 0) {
                c = _a % _b;
                _a = _b;
                _b = c;
            }
            return m * n / _a;
        }

        template<typename _Ty>
        static _Ty NextPowerOf2(_Ty const& _size) {
            const auto power = static_cast<_Ty>(std::log2l(static_cast<long double>(_size))) + 1;
            return static_cast<_Ty>(1) << power;
        }

        template<typename _Ty>
        static bool IsPowerOf2(_Ty const& _size) {
            _Ty mask = 0;
            const auto power = static_cast<uint32_t>(std::log2l(static_cast<long double>(_size)));

            for (uint32_t i = 0; i < power; ++i)
                mask += static_cast<_Ty>(1) << i;

            return !(_size & mask);
        }

        template<typename _Ty>
        static bool EpsilonEqual(_Ty const& _a, _Ty const& _b, _Ty const& _epsilon = static_cast<_Ty>(Constants::Epsilon)) {
            return std::abs(_a - _b) < _epsilon;
        }

        template<typename _Ty>
        static constexpr auto Radians(_Ty const& _degrees) {
            return _degrees * static_cast<_Ty>(0.01745329251994329576923690768489);
        }

        template<typename _Ty>
        static constexpr auto Degrees(_Ty const& _radians) {
            return _radians * static_cast<_Ty>(57.295779513082320876798154814105);
        }

        template<typename _Ty>
        static constexpr auto SmoothDamp(_Ty _current, _Ty _target, _Ty& _currentV, _Ty _smoothTime, _Ty _max, _Ty _delta) {
            _smoothTime = std::max(0.0001f, _smoothTime);
            _Ty num = 2.0f / _smoothTime;
            _Ty num2 = num * _delta;
            _Ty num3 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
            _Ty num4 = _current - _target;
            _Ty num5 = _target;
            _Ty num6 = _max * _smoothTime;
            num4 = std::clamp(num4, -num6, num6);
            _target = _current - num4;
            float num7 = (_currentV + num * num4) * _delta;
            _currentV = (_currentV - num * num7) * num3;
            float num8 = _target + (num4 + num7) * num3;

            if (num5 - _current > 0.0f == num8 > num5) {
                num8 = num5;
                _currentV = (num8 - num5) / _delta;
            }
            return num8;
        }

        template<typename _Ty>
        static constexpr auto Lerp(_Ty const& _a, _Ty const& _b, float _t) {
            _t = std::clamp(_t, 0.0f, 1.0f);
            return  _b * _t + _a * (1.0f - _t);
        }

        template<typename _Ty>
        static constexpr auto Step(_Ty const& _edge, _Ty const& _t) {
            return _t < _edge ? static_cast<_Ty>(0) : static_cast<_Ty>(1);
        }

        template<typename _Ty>
        static constexpr auto Step(_Ty const& _a, _Ty const& _b, float _thresh, float _t) {
            return _t < _thresh ? _a : _b;
        }

        template<typename _Ty>
        static constexpr auto LerpUnclamped(_Ty const& _a, _Ty const& _b, float _t) {
            return  _b * _t + _a * (1.0f - _t);
        }

        template<typename _Ty, typename _Re = float>
        static constexpr _Re Normalize(_Ty const& _value) {
            static_assert(!std::is_floating_point_v<_Ty>, "You can't use this function on float, double, long double");

            return _value < 0 ?
                -static_cast<float>(_value) / std::numeric_limits<_Ty>::min() :
                static_cast<float>(_value) / std::numeric_limits<_Ty>::max();
        }

        template<typename _Ty>
        static _Ty CubicSpline(float _t, _Ty const& _prePoint, _Ty const& _preTangent, _Ty const& _nxtPoint, _Ty const& _nxtTangent) {
            auto t2 = _t * _t;
            auto t3 = _t * t2;

            return (2 * t3 - 3 * t2 + 1) * _prePoint + (t3 - 2 * t2 + _t) * _preTangent + (-2 * t3 + 3 * t2) * _nxtPoint + (t3 - t2) * _nxtTangent;
        }

        static uint16 FloatToHalf(float _f);

        static float HalfToFloat(uint16 _h);

        /**
         * \brief Convert an unsigned integer to float in range [0, 1].
         */
        static float UnpackUnorm(uint32 _value, uint32 _bits = 8) {
            return float(_value) / float((1 << _bits) - 1);
        }

        /**
         * \brief Convert a float in range [0, 1] to an unsigned integer.
         */
        static uint32 PackUnorm(float _value, uint32 _bits = 8) {
            _value = std::clamp(_value, 0.0f, 1.0f);
            return uint32(std::floor(_value * ((1 << _bits) - 1)));
        }

        /**
         * \brief Convert an unsigned integer to float in range [-1, 1].
         */
        static float UnpackSnorm(uint32 _value, uint32 _bits = 8) {
            return UnpackUnorm(_value, _bits) * 2.0f - 1.0f;
        }

        /**
         * \brief Convert a float in range [-1, 1] to an unsigned integer.
         */
        static uint32 PackSnorm(float _value, uint32 _bits = 8) {
            return PackUnorm((_value + 1.0f) * 0.5f, _bits);
        }

        template<typename _Ty>
        static constexpr _Ty Zero() {
            return _Ty(0);
        }
    }
}

#endif

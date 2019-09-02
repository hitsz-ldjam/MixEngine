#pragma once
#ifndef MX_MATH_H_
#define MX_MATH_H_
#include "../Utils/MxGeneralBase.hpp"
#include <cmath>

namespace Mix {
	namespace Math {
		class Constants :public GeneralBase::StaticBase {
		public:
			constexpr static float Epsilon = 0.000001f;
			constexpr static float Pi = 3.141593f;
		};

		template<typename _Pre, typename _Lst>
		auto Align(_Pre const& _size, _Lst const& _alignment) {
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

		template<typename _Ty>
		bool EpsilonEqual(_Ty const& _a, _Ty const& _b, _Ty const& _epsilon = static_cast<_Ty>(Constants::Epsilon)) {
			return std::abs(_a - _b) < _epsilon;
		}

		template<typename _Ty>
		constexpr auto Radians(_Ty const& _degrees) {
			return _degrees * static_cast<_Ty>(0.01745329251994329576923690768489);
		}

		template<typename _Ty>
		constexpr auto Degrees(_Ty const& _radians) {
			return _radians * static_cast<_Ty>(57.295779513082320876798154814105);
		}

		template<typename _Ty>
		constexpr auto SmoothDamp(_Ty _current, _Ty _target, _Ty& _currentV, _Ty _smoothTime, _Ty _max, _Ty _delta) {
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
		constexpr auto Lerp(const _Ty& _a, const _Ty& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			return  _b * _t + _a * (1.0f - _t);
		}

		template<typename _Ty>
		constexpr auto LerpUnclamped(const _Ty& _a, const _Ty& _b, float _t) {
			return  _b * _t + _a * (1.0f - _t);
		}

		template<typename _Ty, typename _Re = float>
		constexpr _Re Normalize(_Ty _value) {
			static_assert(!std::is_floating_point_v<_Ty>, "You can't use this function on float, double, long double");

			return _value < 0 ?
				-static_cast<float>(_value) / std::numeric_limits<_Ty>::min() :
				static_cast<float>(_value) / std::numeric_limits<_Ty>::max();
		}
	}
}

#endif

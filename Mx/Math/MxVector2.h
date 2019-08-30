#pragma once
#ifndef MX_VECTOR2_H_
#define WMX_VECTOR2_H_

#include "../Exceptions/MxExceptions.hpp"
#include <cmath>
#include <boost/format.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/compatibility.hpp>
#include "MxMath.h"

namespace Mix {
	namespace Math {
		template<typename _Ty>
		class Vector3;

		template<typename _Ty>
		class Vector2 {
		public:
			union {
				struct {
					union { _Ty x, r, s; };
					union { _Ty y, g, t; };
				};

				glm::vec<2, _Ty, glm::defaultp> vec;

				_Ty linear[2];
			};

			static const Vector2 Zero;
			static const Vector2 One;
			static const Vector2 Up;
			static const Vector2 Down;
			static const Vector2 Left;
			static const Vector2 Right;

			explicit Vector2(_Ty const& _a = static_cast<_Ty>(0)) :x(_a), y(_a) {}

			explicit Vector2(const glm::vec<2, _Ty, glm::defaultp>& _vec2) :vec(_vec2) {}

			template<typename _T>
			explicit Vector2(_T const& _a) :x(static_cast<_Ty>(_a)), y(static_cast<_Ty>(_a)) {}

			Vector2(_Ty const& _x, _Ty const& _y) :x(_x), y(_y) {}

			template<typename _X, typename _Y>
			Vector2(_X const& _x, _Y const& _y) : x(static_cast<_Ty>(_x)), y(static_cast<_Ty>(_y)) {}

			Vector2(const Vector2& _other) = default;

			template<typename _T>
			Vector2(const Vector2<_T>& _source) :
				x(static_cast<_Ty>(_source.x)), y(static_cast<_Ty>(_source.y)) {
			}

			template<typename _T>
			explicit  Vector2(const Vector3<_T>& _source) :
				x(static_cast<_Ty>(_source.x)), y(static_cast<_Ty>(_source.y)) {
			}

			template<typename _T>
			auto add(const Vector2<_T>& _other) const {
				return Vector2<decltype(x + _other.x)>(x + _other.x, y + _other.y);
			}

			template<typename _T>
			auto add(_T const& _a) const {
				return Vector2<decltype(x + _a)>(x + _a, y + _a);
			}

			template<typename _T>
			auto minus(const Vector2<_T>& _other) const {
				return Vector2<decltype(x - _other.x)>(x - _other.x, y - _other.y);
			}

			template<typename _T>
			auto minus(_T const& _a) const {
				return Vector2<decltype(x - _a)>(x - _a, y - _a);
			}

			template<typename _T>
			auto multiply(const Vector2<_T>& _other) const {
				return Vector2<decltype(x * _other.x)>(x * _other.x, y * _other.y);
			}

			template<typename _T>
			auto multiply(_T const& _a) const {
				return Vector2<decltype(x * _a)>(x * _a, y * _a);
			}

			template<typename _T>
			auto divide(const Vector2<_T>& _other) const {
				return Vector2<decltype(x / _other.x)>(x / _other.x, y / _other.y);
			}

			template<typename _T>
			auto divide(_T const& _a) const {
				return Vector2<decltype(x / _a)>(x / _a, y / _a);
			}

			template<typename _T>
			auto dot(const Vector2<_T>& _other) const {
				return x * _other.x + y * _other.y;
			}

			template<typename _T>
			bool isParallel(const Vector2<_T>& _other) const {
				Vector2<float> norm1 = Vector2<float>(*this).normalize();
				Vector2<float> norm2 = Vector2<float>(_other).normalize();
				return EpsilonEqual(std::abs(norm1.dot(norm2)), 1.0f);
			}

			template<typename _T>
			bool isSameDir(const Vector2<_T>& _other) const {
				Vector2<float> norm1 = Vector2<float>(*this).normalize();
				Vector2<float> norm2 = Vector2<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), 1.0f);
			}

			template<typename _T>
			bool isOppositeDir(const Vector2<_T>& _other) const {
				Vector2<float> norm1 = Vector2<float>(*this).normalize();
				Vector2<float> norm2 = Vector2<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), -1.0f);
			}

			template<typename _T>
			bool isPerpend(const Vector2<_T>& _other) const {
				Vector2<float> norm1 = Vector2<float>(*this).normalize();
				Vector2<float> norm2 = Vector2<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), 0.0f);
			}

			auto sqrLength() const {
				return (x * x + y * y);
			}

			auto length() const {
				return std::sqrt(sqrLength());
			}

			auto normalize() const {
				auto l = length();

				if (l == 0) {
					throw Exception("Can't normalize a zero length vector");
				}

				return this->divide(l);
			}

			template<typename _T>
			auto distance(const Vector2<_T>& _other) const {
				auto dx = x - _other.x;
				auto dy = y - _other.y;
				return std::sqrt(dx * dx + dy * dy);
			}

			std::string toString() const {
				return (boost::format("(%d, %d)") % x % y).str();
			}

			template<typename _T>
			bool operator==(const Vector2<_T>& _other) const {
				return EpsilonEqual(x, _other.x) && EpsilonEqual(y, _other.y);
			}

			template<typename _T>
			bool operator!=(const Vector2<_T>& _other) const {
				return !(*this == _other);
			}

			template<typename _U = _Ty>
			std::enable_if_t<std::is_signed_v<_U>, Vector2> operator-() const {
				return Vector2(-x, -y);
			}

			_Ty const& operator[](const uint32_t _index) const {
				switch (_index) {
				case 0:
					return x;
				case 1:
					return y;
				default:
					throw Exception("Vector2 index out of bounds!");
				}
			}

			_Ty& operator[](const uint32_t _index) {
				return const_cast<_Ty&>(static_cast<const Vector2&>(*this)[_index]);
			}

			template<typename _T>
			Vector2& operator+=(const Vector2<_T>& _right) {
				return *this = add(_right);
			}

			template<typename _T>
			Vector2& operator-=(const Vector2<_T>& _right) {
				return *this = minus(_right);
			}

			template<typename _T>
			Vector2& operator*=(const Vector2<_T>& _right) {
				return *this = multiply(_right);
			}

			template<typename _T>
			Vector2& operator/=(const Vector2<_T>& _right) {
				return *this = divide(_right);
			}

			Vector2& operator+=(const _Ty& _right) {
				return *this = add(Vector2<_Ty>(_right));
			}

			Vector2& operator-=(const _Ty& _right) {
				return *this = minus(Vector2<_Ty>(_right));
			}

			Vector2& operator*=(const _Ty& _right) {
				return *this = multiply(Vector2<_Ty>(_right));
			}

			Vector2& operator/=(const _Ty& _right) {
				return *this = divide(Vector2<_Ty>(_right));
			}

			// ---------- static method ----------

			static Vector2 Lerp(const Vector2& _a, const Vector2& _b, float _t);

			static Vector2 LerpUnclamped(const Vector2& _a, const Vector2& _b, float _t);

			static Vector2 Slerp(const Vector2& _a, const Vector2& _b, float _t);

			static Vector2 SlerpUnclamped(const Vector2& _a, const Vector2& _b, float _t);

			static Vector2 SmoothDamp(const Vector2& _current, const Vector2& _target,
									  Vector2& _currentV, float _smoothTime, float _max, float _delta);
		};

		template <typename _Ty>
		Vector2<_Ty> Vector2<_Ty>::Lerp(const Vector2& _a, const Vector2& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			return Vector2(_b.x * _t + _a.x * (1.0f - _t), _b.y * _t + _a.y * (1.0f - _t));
		}

		template <typename _Ty>
		Vector2<_Ty> Vector2<_Ty>::LerpUnclamped(const Vector2& _a, const Vector2& _b, float _t) {
			return Vector2(_b.x * _t + _a.x * (1.0f - _t), _b.y * _t + _a.y * (1.0f - _t));
		}

		template <typename _Ty>
		Vector2<_Ty> Vector2<_Ty>::Slerp(const Vector2& _a, const Vector2& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector2 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector2((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector2<_Ty> Vector2<_Ty>::SlerpUnclamped(const Vector2& _a, const Vector2& _b, float _t) {
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector2 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector2((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector2<_Ty> Vector2<_Ty>::SmoothDamp(const Vector2& _current, const Vector2& _target, Vector2& _currentV, float _smoothTime, float _max, float _delta) {
			// test
			return Vector2(Math::SmoothDamp(_current.x, _target.x, _currentV.x, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.y, _target.y, _currentV.y, _smoothTime, _max, _delta));
		}

		// Vector<_T1> +|-|*|/ Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(const Vector2<_T1>& _left, const Vector2<_T2>& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector2<_T1>& _left, const Vector2<_T2>& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector2<_T1>& _left, const Vector2<_T2>& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector2<_T1>& _left, const Vector2<_T2>& _right) {
			return _left.divide(_right);
		}

		// _T1 +|* Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(_T1 const& _left, const Vector2<_T2>& _right) {
			return Vector2<_T1>(_left).add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(_T1 const& _left, const Vector2<_T2>& _right) {
			return Vector2<_T1>(_left).multiply(_right);
		}

		// Vector<_T1> +|-|*|/ _T2

		template<typename _T1, typename _T2>
		auto operator+(const Vector2<_T1>& _left, _T2 const& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector2<_T1>& _left, _T2 const& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector2<_T1>& _left, _T2 const& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector2<_T1>& _left, _T2 const& _right) {
			return _left.divide(_right);
		}

		// operators for output
		template<typename _Os, typename _Ty>
		_Os& operator<<(_Os& _os, const Vector2<_Ty>& _v) {
			_os << _v.toString();
			return _os;
		}

		template<typename _Ty>
		Vector2<bool> EpsilonEqual(const Vector2<_Ty>& _a, const Vector2<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Constants::Epsilon)) {
			return Vector2<bool>(std::abs(_a.x - _b.x) < _epsilon, std::abs(_a.y - _b.y) < _epsilon);
		}

		// Commonly used types
		using Vector2f = Vector2<float>;
		using Vector2d = Vector2<double>;
		using Vector2i = Vector2<int32_t>;
		using Vector2ui = Vector2<uint32_t>;
	}
}


#endif

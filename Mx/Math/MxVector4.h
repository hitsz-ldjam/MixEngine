#pragma once
#ifndef MX_VECTOR4_H_
#define WMX_VECTOR4_H_

#include "MxGLMHeader.h"

#include "../Exceptions/MxExceptions.hpp"
#include "MxMath.h"
#include <boost/format.hpp>
#include <glm/vec4.hpp>
#include <cmath>

namespace Mix {
	namespace Math {
		template<typename _Ty>
		class Vector2;

		template<typename _Ty>
		class Vector3;

		template<typename _Ty>
		class Vector4 {
		public:
			union {
				struct {
					union { _Ty x, r, s; };
					union { _Ty y, g, t; };
					union { _Ty z, b, p; };
					union { _Ty w, a, q; };
				};

				glm::vec<4, _Ty, glm::defaultp> vec;

				_Ty linear[4];
			};

			static const Vector4 Zero;
			static const Vector4 One;

			explicit Vector4(_Ty const& _a = static_cast<_Ty>(0)) :x(_a), y(_a), z(_a), w(_a) {}

			explicit Vector4(const glm::vec<4, _Ty, glm::defaultp>& _vec4) :vec(_vec4) {}

			template<typename _T>
			explicit Vector4(_T const& _a) :
				x(static_cast<_Ty>(_a)),
				y(static_cast<_Ty>(_a)),
				z(static_cast<_Ty>(_a)),
				w(static_cast<_Ty>(_a)) {
			}

			Vector4(_Ty const& _x, _Ty const& _y, _Ty const& _z, _Ty const& _w) :x(_x), y(_y), z(_z), w(_w) {}

			template<typename _X, typename _Y, typename _Z, typename _W>
			Vector4(_X const& _x, _Y const& _y, _Z const& _z, _W const& _w) :
				x(static_cast<_Ty>(_x)),
				y(static_cast<_Ty>(_y)),
				z(static_cast<_Ty>(_z)),
				w(static_cast<_Ty>(_w)) {
			}

			Vector4(const Vector4& _other) = default;

			template<typename _T>
			Vector4(const Vector4<_T>& _source) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_source.z)),
				w(static_cast<_Ty>(_source.w)) {
			}

			template<typename _T, typename _Z, typename _W>
			explicit  Vector4(const Vector2<_T>& _source, _Z const& _z, _W const& _w) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_z)),
				w(static_cast<_Ty>(_w)) {
			}

			template<typename _T, typename _W>
			explicit Vector4(const Vector3<_T>& _source, _W const& _w) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_source.z)),
				w(static_cast<_Ty>(_w)) {
			}

			template<typename _T>
			auto add(const Vector4<_T>& _other) const {
				return Vector4<decltype(x + _other.x)>(x + _other.x, y + _other.y, z + _other.z, w + _other.w);
			}

			template<typename _T>
			auto add(_T const& _a) const {
				return Vector4<decltype(x + _a)>(x + _a, y + _a, z + _a, w + _a);
			}

			template<typename _T>
			auto minus(const Vector4<_T>& _other) const {
				return Vector4<decltype(x - _other.x)>(x - _other.x, y - _other.y, z - _other.z, w - _other.w);
			}

			template<typename _T>
			auto minus(_T const& _a) const {
				return Vector4<decltype(x - _a)>(x - _a, y - _a, z - _a, w - _a);
			}

			template<typename _T>
			auto multiply(const Vector4<_T>& _other) const {
				return Vector4<decltype(x * _other.x)>(x * _other.x, y * _other.y, z * _other.z, w * _other.w);
			}

			template<typename _T>
			auto multiply(_T const& _a) const {
				return Vector4<decltype(x * _a)>(x * _a, y * _a, z * _a, w * _a);
			}

			template<typename _T>
			auto divide(const Vector4<_T>& _other) const {
				return Vector4<decltype(x / _other.x)>(x / _other.x, y / _other.y, z / _other.z, w / _other.w);
			}

			template<typename _T>
			auto divide(_T const& _a) const {
				return Vector4<decltype(x / _a)>(x / _a, y / _a, z / _a, w / _a);
			}

			auto sqrLength() const {
				return (x * x + y * y + z * z + w * w);
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

			std::string toString() const {
				return (boost::format("(%d, %d, %d, %d)") % x % y % z % w).str();
			}

			template<typename _T>
			bool operator==(const Vector4<_T>& _other) const {
				return EpsilonEqual(x, _other.x) && EpsilonEqual(y, _other.y) && EpsilonEqual(z, _other.z) && EpsilonEqual(w, _other.w);
			}

			template<typename _T>
			bool operator!=(const Vector4<_T>& _other) const {
				return !(*this == _other);
			}

			template<typename _U = _Ty>
			std::enable_if_t<std::is_signed_v<_U>, Vector4> operator-() const {
				return Vector4(-x, -y, -z, -w);
			}

			_Ty const& operator[](const uint32_t _index) const {
				switch (_index) {
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				case 3:
					return w;
				default:
					throw Exception("Vector4 index out of bounds!");
				}
			}

			_Ty& operator[](const uint32_t _index) {
				return const_cast<_Ty&>(static_cast<const Vector4&>(*this)[_index]);
			}

			template<typename _T>
			Vector4& operator+=(const Vector4<_T>& _right) {
				return *this = add(_right);
			}

			template<typename _T>
			Vector4& operator-=(const Vector4<_T>& _right) {
				return *this = minus(_right);
			}

			template<typename _T>
			Vector4& operator*=(const Vector4<_T>& _right) {
				return *this = multiply(_right);
			}

			template<typename _T>
			Vector4& operator/=(const Vector4<_T>& _right) {
				return *this = divide(_right);
			}

			Vector4& operator+=(const _Ty& _right) {
				return *this = add(Vector4<_Ty>(_right));
			}

			Vector4& operator-=(const _Ty& _right) {
				return *this = minus(Vector4<_Ty>(_right));
			}

			Vector4& operator*=(const _Ty& _right) {
				return *this = multiply(Vector4<_Ty>(_right));
			}

			Vector4& operator/=(const _Ty& _right) {
				return *this = divide(Vector4<_Ty>(_right));
			}

			static Vector4 Lerp(const Vector4& _a, const Vector4& _b, float _t);

			static Vector4 LerpUnclamped(const Vector4& _a, const Vector4& _b, float _t);

			static Vector4 Slerp(const Vector4& _a, const Vector4& _b, float _t);

			static Vector4 SlerpUnclamped(const Vector4& _a, const Vector4& _b, float _t);

			static Vector4 SmoothDamp(const Vector4& _current, const Vector4& _target,
									  Vector4& _currentV, float _smoothTime, float _max, float _delta);

		};

		template <typename _Ty>
		Vector4<_Ty> Vector4<_Ty>::Lerp(const Vector4& _a, const Vector4& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			return Vector4(_b.x * _t + _a.x * (1.0f - _t),
						   _b.y * _t + _a.y * (1.0f - _t),
						   _b.z * _t + _a.z * (1.0f - _t),
						   _b.w * _t + _a.w * (1.0f - _t));
		}

		template <typename _Ty>
		Vector4<_Ty> Vector4<_Ty>::LerpUnclamped(const Vector4& _a, const Vector4& _b, float _t) {
			return Vector4(_b.x * _t + _a.x * (1.0f - _t),
						   _b.y * _t + _a.y * (1.0f - _t),
						   _b.z * _t + _a.z * (1.0f - _t),
						   _b.w * _t + _a.w * (1.0f - _t));
		}

		template <typename _Ty>
		Vector4<_Ty> Vector4<_Ty>::Slerp(const Vector4& _a, const Vector4& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector4 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector4((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector4<_Ty> Vector4<_Ty>::SlerpUnclamped(const Vector4& _a, const Vector4& _b, float _t) {
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector4 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector4((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector4<_Ty> Vector4<_Ty>::SmoothDamp(const Vector4& _current, const Vector4& _target, Vector4& _currentV, float _smoothTime, float _max, float _delta) {
			// test
			return Vector4(Math::SmoothDamp(_current.x, _target.x, _currentV.x, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.y, _target.y, _currentV.y, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.z, _target.z, _currentV.z, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.w, _target.w, _currentV.w, _smoothTime, _max, _delta));
		}

		// Vector<_T1> +|-|*|/ Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(const Vector4<_T1>& _left, const Vector4<_T2>& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector4<_T1>& _left, const Vector4<_T2>& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector4<_T1>& _left, const Vector4<_T2>& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector4<_T1>& _left, const Vector4<_T2>& _right) {
			return _left.divide(_right);
		}

		// _T1 +|* Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(_T1 const& _left, const Vector4<_T2>& _right) {
			return Vector4<_T1>(_left).add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(_T1 const& _left, const Vector4<_T2>& _right) {
			return Vector4<_T1>(_left).multiply(_right);
		}

		// Vector<_T1> +|-|*|/ _T2

		template<typename _T1, typename _T2>
		auto operator+(const Vector4<_T1>& _left, _T2 const& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector4<_T1>& _left, _T2 const& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector4<_T1>& _left, _T2 const& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector4<_T1>& _left, _T2 const& _right) {
			return _left.divide(_right);
		}

		// operators for output
		template<typename _Os, typename _Ty>
		_Os& operator<<(_Os& _os, const Vector4<_Ty>& _v) {
			_os << _v.toString();
			return _os;
		}

		template<typename _Ty>
		Vector4<bool> EpsilonEqual(const Vector4<_Ty>& _a, const Vector4<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Constants::Epsilon)) {
			return Vector4<bool>(std::abs(_a.x - _b.x) < _epsilon,
								 std::abs(_a.y - _b.y) < _epsilon,
								 std::abs(_a.z - _b.z) < _epsilon,
								 std::abs(_a.w - _b.w) < _epsilon);
		}

		// Commonly used types
		using Vector4f = Vector4<float>;
		using Vector4d = Vector4<double>;
		using Vector4i = Vector4<int32_t>;
		using Vector4ui = Vector4<uint32_t>;
	}
}

#endif

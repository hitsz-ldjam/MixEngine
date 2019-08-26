#pragma once
#ifndef MX_VECTOR3_H_
#define WMX_VECTOR3_H_

#include "../Exceptions/MxExceptions.hpp"
#include <boost/format.hpp>
#include <glm/vec3.hpp>
#include <cmath>
#include "MxMath.h"

namespace Mix {
	namespace Math {
		template<typename _Ty>
		class Vector2;

		template<typename _Ty>
		class Vector4;

		template<typename _Ty>
		class Vector3 {
		public:
			union {
				struct {
					union { _Ty x, r, s; };
					union { _Ty y, g, t; };
					union { _Ty z, b, p; };
				};

				glm::vec<3, _Ty, glm::defaultp> vec;

				_Ty linear[3];
			};

			static const Vector3 Zero;
			static const Vector3 One;
			static const Vector3 Up;
			static const Vector3 Down;
			static const Vector3 Left;
			static const Vector3 Right;
			static const Vector3 Forward;
			static const Vector3 Back;

			/**
			 * \brief Construct a Vector3 with x = y = z = _a or default 0
			 */
			explicit Vector3(_Ty const& _a = static_cast<_Ty>(0)) :x(_a), y(_a), z(_a) {}

			/**
			 * \brief Convert glm::vec<3, T, Q> to Vector3
			 */
			explicit Vector3(const glm::vec<3, _Ty, glm::defaultp>& _vec3) :vec(_vec3) {}

			/**
			 * \brief Construct a Vector3 with x = y = z = _a
			 */
			template<typename _T>
			explicit Vector3(_T const& _a) :
				x(static_cast<_Ty>(_a)),
				y(static_cast<_Ty>(_a)),
				z(static_cast<_Ty>(_a)) {
			}

			/**
			 * \brief Construct a Vector3 with given x, y, z components.
			 */
			Vector3(_Ty const& _x, _Ty const& _y, _Ty const& _z) :x(_x), y(_y), z(_z) {}

			/**
			 * \brief Construct a Vector3 with given x, y, z components.
			 */
			template<typename _X, typename _Y, typename _Z>
			Vector3(_X const& _x, _Y const& _y, _Z const& _z) :
				x(static_cast<_Ty>(_x)),
				y(static_cast<_Ty>(_y)),
				z(static_cast<_Ty>(_z)) {
			}

			/**
			 * \brief Copy from another Vector3
			 */
			Vector3(const Vector3& _other) = default;

			/**
			 * \brief Copy from another Vector3 with different type
			 */
			template<typename _T>
			Vector3(const Vector3<_T>& _source) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_source.z)) {
			}

			/**
			 * \brief Construct a Vector3 with a given Vector2 and set z to _z
			 */
			template<typename _T, typename _Z>
			explicit  Vector3(const Vector2<_T>& _source, _Z const& _z) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_z)) {
			}

			/**
			 * \brief Construct a Vector3 with a given Vector4, w component of given Vector4 will be ignored
			 */
			template<typename _T>
			explicit Vector3(const Vector4<_T>& _source) :
				x(static_cast<_Ty>(_source.x)),
				y(static_cast<_Ty>(_source.y)),
				z(static_cast<_Ty>(_source.z)) {
			}

			/**
			 * \brief Add corresponding components together and return the new Vector3
			 */
			template<typename _T>
			auto add(const Vector3<_T>& _other) const {
				return Vector3<decltype(x + _other.x)>(x + _other.x, y + _other.y, z + _other.z);
			}

			/**
			 * \brief Add _a to each component and return the new Vector3
			 */
			template<typename _T>
			auto add(_T const& _a) const {
				return Vector3<decltype(x + _a)>(x + _a, y + _a, z + _a);
			}

			/**
			 * \brief Subtract each component of _other from this and return the new Vector3
			 */
			template<typename _T>
			auto minus(const Vector3<_T>& _other) const {
				return Vector3<decltype(x - _other.x)>(x - _other.x, y - _other.y, z - _other.z);
			}

			/**
			 * \brief Subtract _a from each component of this and return the new Vector3
			 */
			template<typename _T>
			auto minus(_T const& _a) const {
				return Vector3<decltype(x - _a)>(x - _a, y - _a, z - _a);
			}

			/**
			 * \brief Multiply each component of this by corresponding components of _other and return the new Vector3
			 */
			template<typename _T>
			auto multiply(const Vector3<_T>& _other) const {
				return Vector3<decltype(x * _other.x)>(x * _other.x, y * _other.y, z * _other.z);
			}

			/**
			 * \brief Multiply each component of this _a and return the new Vector3
			 */
			template<typename _T>
			auto multiply(_T const& _a) const {
				return Vector3<decltype(x * _a)>(x * _a, y * _a, z * _a);
			}

			/**
			 * \brief Divide each component of this by corresponding components of _other and return the new Vector3
			 */
			template<typename _T>
			auto divide(const Vector3<_T>& _other) const {
				return Vector3<decltype(x / _other.x)>(x / _other.x, y / _other.y, z / _other.z);
			}

			/**
			 * \brief Divide each component of this by _a and return the new Vector3
			 */
			template<typename _T>
			auto divide(_T const& _a) const {
				return Vector3<decltype(x / _a)>(x / _a, y / _a, z / _a);
			}


			/**
			 * \brief Dot Product of this and _other
			 */
			template<typename _T>
			auto dot(const Vector3<_T>& _other) const {
				return x * _other.x + y * _other.y + z * _other.z;
			}

			/**
			 * \brief Cross Product of this and _other
			 */
			template<typename _T>
			auto cross(const Vector3<_T>& _other) const {
				return Vector3<decltype(x * _other.x)>(
					y * _other.z - z * _other.y,
					_other.x * z - _other.z * x,
					x * _other.y - y * _other.x);
			}

			template<typename _T>
			bool isParallel(const Vector3<_T>& _other) const {
				Vector3<float> norm1 = Vector3<float>(*this).normalize();
				Vector3<float> norm2 = Vector3<float>(_other).normalize();
				return EpsilonEqual(std::abs(norm1.dot(norm2)), 1.0f);
			}

			template<typename _T>
			bool isSameDir(const Vector3<_T>& _other) const {
				Vector3<float> norm1 = Vector3<float>(*this).normalize();
				Vector3<float> norm2 = Vector3<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), 1.0f);
			}

			template<typename _T>
			bool isOppositeDir(const Vector3<_T>& _other) const {
				Vector3<float> norm1 = Vector3<float>(*this).normalize();
				Vector3<float> norm2 = Vector3<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), -1.0f);
			}

			template<typename _T>
			bool isPerpend(const Vector3<_T>& _other) const {
				Vector3<float> norm1 = Vector3<float>(*this).normalize();
				Vector3<float> norm2 = Vector3<float>(_other).normalize();
				return EpsilonEqual(norm1.dot(norm2), 0.0f);
			}

			auto sqrLength() const {
				return (x * x + y * y + z * z);
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
			auto distance(const Vector3<_T>& _other) const {
				auto dx = x - _other.x;
				auto dy = y - _other.y;
				auto dz = z - _other.z;
				return std::sqrt(dx * dx + dy * dy + dz * dz);
			}

			std::string toString() const {
				return (boost::format("Vector3(%d, %d, %d)") % x % y % z).str();
			}

			template<typename _T>
			bool operator==(const Vector3<_T>& _other) const {
				return EpsilonEqual(x, _other.x) && EpsilonEqual(y, _other.y) && EpsilonEqual(z, _other.z);
			}

			template<typename _T>
			bool operator!=(const Vector3<_T>& _other) const {
				return !(*this == _other);
			}

			template<typename _U = _Ty>
			std::enable_if_t<std::is_signed_v<_U>, Vector3> operator-() const {
				return Vector3(-x, -y, -z);
			}

			_Ty const& operator[](const uint32_t _index) const {
				switch (_index) {
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				default:
					throw Exception("Vector3 index out of bounds!");
				}
			}

			_Ty& operator[](const uint32_t _index) {
				return const_cast<_Ty&>(static_cast<const Vector3&>(*this)[_index]);
			}

			template<typename _T>
			Vector3& operator+=(const Vector3<_T>& _right) {
				return *this = add(_right);
			}

			template<typename _T>
			Vector3& operator-=(const Vector3<_T>& _right) {
				return *this = minus(_right);
			}

			template<typename _T>
			Vector3& operator*=(const Vector3<_T>& _right) {
				return *this = multiply(_right);
			}

			template<typename _T>
			Vector3& operator/=(const Vector3<_T>& _right) {
				return *this = divide(_right);
			}

			Vector3& operator+=(const _Ty& _right) {
				return *this = add(Vector3<_Ty>(_right));
			}

			Vector3& operator-=(const _Ty& _right) {
				return *this = minus(Vector3<_Ty>(_right));
			}

			Vector3& operator*=(const _Ty& _right) {
				return *this = multiply(Vector3<_Ty>(_right));
			}

			Vector3& operator/=(const _Ty& _right) {
				return *this = divide(Vector3<_Ty>(_right));
			}

			// ---------- static method ----------

			static Vector3 Lerp(const Vector3& _a, const Vector3& _b, float _t);

			static Vector3 LerpUnclamped(const Vector3& _a, const Vector3& _b, float _t);

			static Vector3 Slerp(const Vector3& _a, const Vector3& _b, float _t);

			static Vector3 SlerpUnclamped(const Vector3& _a, const Vector3& _b, float _t);

			static Vector3 SmoothDamp(const Vector3& _current, const Vector3& _target,
									  Vector3& _currentV, float _smoothTime, float _max, float _delta);

		};

		template <typename _Ty>
		Vector3<_Ty> Vector3<_Ty>::Lerp(const Vector3& _a, const Vector3& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			return Vector3(_b.x * _t + _a.x * (1.0f - _t),
						   _b.y * _t + _a.y * (1.0f - _t),
						   _b.z * _t + _a.z * (1.0f - _t));
		}

		template <typename _Ty>
		Vector3<_Ty> Vector3<_Ty>::LerpUnclamped(const Vector3& _a, const Vector3& _b, float _t) {
			return Vector3(_b.x * _t + _a.x * (1.0f - _t), 
						   _b.y * _t + _a.y * (1.0f - _t),
						   _b.z * _t + _a.z * (1.0f - _t));
		}

		template <typename _Ty>
		Vector3<_Ty> Vector3<_Ty>::Slerp(const Vector3& _a, const Vector3& _b, float _t) {
			_t = std::clamp(_t, 0.0f, 1.0f);
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector3 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector3((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector3<_Ty> Vector3<_Ty>::SlerpUnclamped(const Vector3& _a, const Vector3& _b, float _t) {
			float dot = _a.dot(_b);
			float theta = std::acos(dot) * _t;
			Vector3 relativeVec = _b - _a * dot;
			relativeVec = relativeVec.normalize();
			return Vector3((_a*std::cos(theta)) + (relativeVec*std::sin(theta)));
		}

		template <typename _Ty>
		Vector3<_Ty> Vector3<_Ty>::SmoothDamp(const Vector3& _current, const Vector3& _target, Vector3& _currentV, float _smoothTime, float _max, float _delta) {
			// test
			return Vector3(Math::SmoothDamp(_current.x, _target.x, _currentV.x, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.y, _target.y, _currentV.y, _smoothTime, _max, _delta),
						   Math::SmoothDamp(_current.z, _target.z, _currentV.z, _smoothTime, _max, _delta));
		}

		// Vector<_T1> +|-|*|/ Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(const Vector3<_T1>& _left, const Vector3<_T2>& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector3<_T1>& _left, const Vector3<_T2>& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector3<_T1>& _left, const Vector3<_T2>& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector3<_T1>& _left, const Vector3<_T2>& _right) {
			return _left.divide(_right);
		}

		// _T1 +|-|*|/ Vector<_T2>

		template<typename _T1, typename _T2>
		auto operator+(_T1 const& _left, const Vector3<_T2>& _right) {
			return Vector3<_T1>(_left).add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(_T1 const& _left, const Vector3<_T2>& _right) {
			return Vector3<_T1>(_left).minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(_T1 const& _left, const Vector3<_T2>& _right) {
			return Vector3<_T1>(_left).multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(_T1 const& _left, const Vector3<_T2>& _right) {
			return Vector3<_T1>(_left).divide(_right);
		}

		// Vector<_T1> +|-|*|/ _T2

		template<typename _T1, typename _T2>
		auto operator+(const Vector3<_T1>& _left, _T2 const& _right) {
			return _left.add(_right);
		}

		template<typename _T1, typename _T2>
		auto operator-(const Vector3<_T1>& _left, _T2 const& _right) {
			return _left.minus(_right);
		}

		template<typename _T1, typename _T2>
		auto operator*(const Vector3<_T1>& _left, _T2 const& _right) {
			return _left.multiply(_right);
		}

		template<typename _T1, typename _T2>
		auto operator/(const Vector3<_T1>& _left, _T2 const& _right) {
			return _left.divide(_right);
		}

		// operators for output
		template<typename _Os, typename _Ty>
		_Os& operator<<(_Os& _os, const Vector3<_Ty>& _v) {
			_os << _v.toString();
			return _os;
		}

		template<typename _Ty>
		Vector3<bool> EpsilonEqual(const Vector3<_Ty>& _a, const Vector3<_Ty>& _b, _Ty const& _epsilon = static_cast<_Ty>(Constants::Epsilon)) {
			return Vector3<bool>(std::abs(_a.x - _b.x) < _epsilon,
								 std::abs(_a.y - _b.y) < _epsilon,
								 std::abs(_a.z - _b.z) < _epsilon);
		}

		// Commonly used types
		using Vector3f = Vector3<float>;
		using Vector3d = Vector3<double>;
		using Vector3i = Vector3<int32_t>;
		using Vector3ui = Vector3<uint32_t>;
	}
}

#endif

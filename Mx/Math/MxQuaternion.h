#pragma once
#ifndef MX_QUATERNION_H_
#define MX_QUATERNION_H_

#include "MxVector3.h"
#include "MxVector4.h"
#include <glm/gtc/quaternion.hpp>

namespace Mix {
	namespace Math {
		class Matrix4;

		class Quaternion {
		public:
			union {
				struct {
					float x, y, z, w;
				};

				glm::quat qua;
			};

			static const Quaternion Identity;

			Quaternion() :x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

			explicit Quaternion(const glm::quat& _quat) :qua(_quat) {}

			Quaternion(const float _w, const float _x, const float _y, const float _z);

			template<typename _W, typename _X, typename _Y, typename _Z>
			Quaternion(_W const& _w, _X const& _x, _Y const& _y, _Z const& _z) :Quaternion(static_cast<float>(_w), static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)) {}

			explicit Quaternion(const Vector4f& _source);

			explicit Quaternion(const float _source[4]);

			explicit Quaternion(const Matrix4& _source);

			Quaternion multiply(const Quaternion& _other) const;

			Vector3f multiply(const Vector3f& _point) const;

			float dot(const Quaternion& _other) const;

			Quaternion lerp(const Quaternion& _other, const float _step) const;

			Quaternion slerp(const Quaternion& _other, const float _step) const;

			Quaternion inverse() const;

			float length() const;

			Quaternion normalize() const;

			Matrix4 toMatrix() const;

			Vector3f toEuler() const;

			void toAxisAngle(float& _angle, Vector3f& _axis) const;

			float getAngle() const;

			Vector3f getAxis() const;

			bool operator==(const Quaternion& _other) const {
				return x == _other.x && y == _other.y && z == _other.z && w == _other.w;
			}

			bool operator!=(const Quaternion& _other) const {
				return !(*this == _other);
			}

			const float& operator[](const uint32_t _index) const;

			float& operator[](const uint32_t _index) {
				return const_cast<float&>(static_cast<const Quaternion&>(*this)[_index]);
			}

			Quaternion operator-() const {
				return Quaternion(-w, -x, -y, -z);
			}

			Quaternion operator*(const Quaternion& _right) const;

			Quaternion& operator*=(const Quaternion& _other);

			Vector3f operator*(const Vector3f& _point) const;

			std::string toString() const;

			static Quaternion Euler(const float _xAngle, const float _yAngle, const float _zAngle);

			static Quaternion Euler(const Vector3f& _euler);

			static Quaternion AngleAxis(const float _angle, const Vector3f& _axis);

			static Quaternion FromToRotation(const Vector3f& _fromDir, const Vector3f& _toDir);

			static Quaternion LookRotation(const Vector3f& _forward, const Vector3f& _up = Vector3f::Up);

#ifdef MX_QUATERNION_MORE_OPERATION
		public:
#else
		private:
#endif // MX_QUATERNION_MORE_OPERATION
			Quaternion scale(const float _other) const;

			Quaternion add(const Quaternion& _other) const;

			Quaternion minus(const Quaternion& _other) const;

			Quaternion operator+(const Quaternion& _right) const;

			Quaternion operator-(const Quaternion& _right) const;

			Quaternion operator/(const Quaternion& _right) const;

			Quaternion operator*(const float _a) const;

			Quaternion operator/(const float _a) const;

			Quaternion& operator*=(const float _a);

#ifdef MX_QUATERNION_MORE_OPERATION
			friend Quaternion operator*(const float _a, const Quaternion& _q);
#endif // MX_QUATERNION_MORE_OPERATION

		};

		template<typename _Os>
		_Os& operator<<(_Os& _os, const Quaternion& _q) {
			_os << _q.toString();
			return _os;
		}
	}
}

#endif

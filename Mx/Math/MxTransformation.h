#pragma once
#ifndef MX_MATH_TRANSFORM_H_
#define MX_MATH_TRANSFORM_H_

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Mix {
	namespace Math {
		class Transform {
		public:
			explicit Transform(const glm::vec3& _position = glm::zero<glm::vec3>(),
			                   const glm::quat& _rotation = glm::identity<glm::quat>(),
			                   const glm::vec3& _scale    = glm::one<glm::vec3>())
				: mPosition(_position),
				  mRotation(_rotation),
				  mScale(_scale),
				  mMatrix(1.0f) {
			}

			Transform& setPosition(const glm::vec3& _position) {
				if (mPosition != _position) {
					mPosition = _position;
					mChanged  = true;
				}
				return *this;
			}

			Transform& setRotation(const glm::quat& _rotation) {
				if (mRotation != _rotation) {
					mRotation = _rotation;
					mChanged  = true;
				}
				return *this;
			}

			Transform& setScale(const glm::vec3& _scale) {
				if (mScale != _scale) {
					mScale   = _scale;
					mChanged = true;
				}
				return *this;
			}

			const glm::vec3& getPosition() const {
				return mPosition;
			}

			const glm::quat& getRotation() const {
				return mRotation;
			}

			const glm::vec3& getScale() const {
				return mScale;
			}

			Transform& fromMat4(const glm::mat4& _mat) {
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(_mat, mScale, mRotation, mPosition, skew, perspective);
				mMatrix = _mat;
				return *this;
			}

			static glm::mat4 MatrixFromTRS(const glm::vec3& _translate, const glm::quat& _rotation,
			                               const glm::vec3& _scale) {

				glm::mat4 result(1.0f);

				// scale
				result[0] = result[0] * _scale[0];
				result[1] = result[1] * _scale[1];
				result[2] = result[2] * _scale[2];

				//rotation
				result = toMat4(_rotation) * result;

				//translate
				result[3] = result[0] * _translate[0] +
				            result[1] * _translate[1] +
				            result[2] * _translate[2] +
				            result[3];

				return result;
			}

			glm::mat4 parentMatrix() const {
				if (mChanged) {
					mMatrix  = MatrixFromTransform(mPosition, mRotation, mScale);
					mChanged = false;
				}
				return mMatrix;
			}

			Transform multiply(const Transform& _other) const {
				return Transform(glm::vec3(parentMatrix() * glm::vec4(_other.getPosition(), 1.0f)),
				                 mRotation * _other.mRotation,
				                 mScale * _other.mScale);
			}

			bool operator==(const Transform& _other) const {
				return mPosition == _other.mPosition &&
				       mRotation == _other.mRotation &&
				       mScale == _other.mScale;
			}

			bool operator!=(const Transform& _other) const {
				return !(*this == _other);
			}

			Transform& operator*=(const Transform& _other) {
				return *this = multiply(_other);
			}

			friend Transform operator*(const Transform& _left, const Transform& _right) {
				return _left.multiply(_right);
			}

		private:
			glm::vec3 mPosition;
			glm::quat mRotation;
			glm::vec3 mScale;

			mutable glm::mat4 mMatrix;
			mutable bool mChanged = true;

			static glm::mat4 MatrixFromTransform(const glm::vec3& _position, const glm::quat& _rotation,
			                                     const glm::vec3& _scale) {
				// Set rotation
				auto result = glm::toMat4(_rotation);

				// Set translate
				result[3].x = _position.x;
				result[3].y = _position.y;
				result[3].z = _position.z;

				// Set scale
				result[0] = result[0] * _scale[0];
				result[1] = result[1] * _scale[1];
				result[2] = result[2] * _scale[2];

				return result;
			}
		};
	}
}
#endif

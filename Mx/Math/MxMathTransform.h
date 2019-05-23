#pragma once
#ifndef MX_MATH_TRANSFORM_H_
#define MX_MATH_TRANSFORM_H_

// macro for glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Mix {
    namespace Math {
        using namespace glm;

        class Transform {
        public:
            explicit Transform(const vec3& _position = zero<vec3>(),
                               const quat& _rotation = identity<quat>(),
                               const vec3& _scale = one<vec3>())
                :mPosition(_position),
                mRotation(_rotation),
                mScale(_scale),
                mMatrix(1.0f) {
            }

            Transform& setPosition(const vec3& _position) {
                if (mPosition != _position) {
                    mPosition = _position;
                    mChanged = true;
                }
                return *this;
            }

            Transform& setRotation(const quat& _rotation) {
                if (mRotation != _rotation) {
                    mRotation = _rotation;
                    mChanged = true;
                }
                return *this;
            }

            Transform& setScale(const vec3& _scale) {
                if (mScale != _scale) {
                    mScale = _scale;
                    mChanged = true;
                }
                return *this;
            }

            const vec3& getPosition() const {
                return mPosition;
            }

            const quat& getRotation() const {
                return mRotation;
            }

            const vec3& getScale() const {
                return mScale;
            }

            Transform& fromMat4(const mat4& _mat) {
                vec3 skew;
                vec4 perspective;
                decompose(_mat, mScale, mRotation, mPosition, skew, perspective);
                mMatrix = _mat;
                return *this;
            }

            static mat4 MatrixFromTRS(const vec3& _position, const quat& _rotation, const vec3& _scale) {

                mat4 result(1.0f);
                
                // scale
                result[0] = result[0] * _scale[0];
                result[1] = result[1] * _scale[1];
                result[2] = result[2] * _scale[2];

                //rotation
                result = toMat4(_rotation) * result;

                //translate
                result[3] = result[0] * _position[0] +
                    result[1] * _position[1] +
                    result[2] * _position[2] +
                    result[3];

                return result;
            }

            mat4 getMatrix() const {
                if (mChanged) {
                    mMatrix = MatrixFromTRS(mPosition, mRotation, mScale);
                    mChanged = false;
                }
                return mMatrix;
            }

            Transform multiply(const Transform& _other) const {
                return Transform(vec3(getMatrix() * vec4(_other.getPosition(), 1.0f)),
                                 mRotation*_other.mRotation,
                                 mScale*_other.mScale);
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
            vec3 mPosition;
            quat mRotation;
            vec3 mScale;

            mutable mat4 mMatrix;
            mutable bool mChanged = true;
        };
    }
}
#endif

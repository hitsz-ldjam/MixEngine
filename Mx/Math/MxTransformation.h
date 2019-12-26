#pragma once
#ifndef MX_MATH_TRANSFORMATION_H_
#define MX_MATH_TRANSFORMATION_H_

#include "MxGLMHeader.h"

#include "MxVector3.h"
#include "MxQuaternion.h"
#include "MxMatrix4.h"

namespace Mix {
    struct TRS {
        Vector3f position;
        Quaternion rotation;
        Vector3f scale;

        TRS() :
            position(Vector3f::Zero),
            rotation(Quaternion::Identity),
            scale(Vector3f::One) {
        }

        TRS(const Vector3f& _position,
            const Quaternion& _rotation,
            const Vector3f& _scale) :
            position(_position),
            rotation(_rotation),
            scale(_scale) {
        }
    };

    class Transformation {
    public:
        Transformation() :
            mPosition(Vector3f::Zero),
            mRotation(Quaternion::Identity),
            mScale(Vector3f::One) {
        }

        Transformation(const Vector3f& _position,
                       const Quaternion& _rotation,
                       const Vector3f& _scale)
            : mPosition(_position),
            mRotation(_rotation),
            mScale(_scale),
            mMatrix(1.0f) {
        }

        Transformation& setPosition(const Vector3f& _position) {
            if (mPosition != _position) {
                mPosition = _position;
                mChanged = true;
            }
            return *this;
        }

        Transformation& setRotation(const Quaternion& _rotation) {
            if (mRotation != _rotation) {
                mRotation = _rotation;
                mChanged = true;
            }
            return *this;
        }

        Transformation& setScale(const Vector3f& _scale) {
            if (mScale != _scale) {
                mScale = _scale;
                mChanged = true;
            }
            return *this;
        }

        const Vector3f& getPosition() const {
            return mPosition;
        }

        const Quaternion& getRotation() const {
            return mRotation;
        }

        const Vector3f& getScale() const {
            return mScale;
        }

        Transformation& fromMat4(const Matrix4& _mat) {
            Matrix4::Decompose(_mat, mPosition, mRotation, mScale);
            mMatrix = _mat;
            return *this;
        }

        static Matrix4 MatrixFromTRS(const Vector3f& _translate, const Quaternion& _rotation,
                                     const Vector3f& _scale) {

            Matrix4 result(1.0f);

            // scale
            result[0] = result[0] * _scale[0];
            result[1] = result[1] * _scale[1];
            result[2] = result[2] * _scale[2];

            //rotation
            result = Quaternion(_rotation).toMatrix() * result;

            //translate
            result[3] = result[0] * _translate[0] +
                result[1] * _translate[1] +
                result[2] * _translate[2] +
                result[3];

            return result;
        }

        Matrix4 parentMatrix() const {
            if (mChanged) {
                mMatrix = Matrix4::TRS(mPosition, mRotation, mScale);
                mChanged = false;
            }
            return mMatrix;
        }

        Transformation multiply(const Transformation& _other) const {
            return Transformation(parentMatrix().multiplyPoint(_other.getPosition()),
                                  mRotation * _other.mRotation,
                                  mScale * _other.mScale);
        }

        bool operator==(const Transformation& _other) const {
            return mPosition == _other.mPosition &&
                mRotation == _other.mRotation &&
                mScale == _other.mScale;
        }

        bool operator!=(const Transformation& _other) const {
            return !(*this == _other);
        }

        Transformation& operator*=(const Transformation& _other) {
            return *this = multiply(_other);
        }

        friend Transformation operator*(const Transformation& _left, const Transformation& _right) {
            return _left.multiply(_right);
        }

    private:
        Vector3f mPosition;
        Quaternion mRotation;
        Vector3f mScale;

        mutable Matrix4 mMatrix;
        mutable bool mChanged = true;
    };

}
#endif

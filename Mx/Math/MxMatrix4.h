#pragma once
#ifndef MX_MATRIX_H_
#define MX_MATRIX_H_

#include "MxGLMHeader.h"

#include "MxVector4.h"
#include "MxVector3.h"
#include "MxQuaternion.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Mix {
    class Matrix4 {
    public:
        union {
            Vector4f cols[4];
            float linear[16];
            glm::mat<4, 4, float, glm::defaultp> mat;
        };

        static const Matrix4 Identity;
        static const Matrix4 Zero;

        explicit Matrix4(const float _diagonal = 1.0f);

        explicit Matrix4(const float _source[16]);

        explicit Matrix4(const glm::mat<4, 4, float, glm::defaultp>& _mat4) :mat(_mat4) {}

        explicit Matrix4(const Vector4f _source[4]) :
            cols{ _source[0],_source[1],_source[2],_source[3] } {
        }

        Matrix4(const float _a00, const float _a01, const float _a02, const float _a03,
                const float _a10, const float _a11, const float _a12, const float _a13,
                const float _a20, const float _a21, const float _a22, const float _a23,
                const float _a30, const float _a31, const float _a32, const float _a33);

        Matrix4 add(const Matrix4& _other) const;

        Matrix4 minus(const Matrix4& _other) const;

        Matrix4 multiply(const Matrix4& _other) const;

        Vector4f multiply(const Vector4f& _vector) const;

        Vector3f multiplyPoint(const Vector3f& _point) const;

        Vector3f multiplyVector(const Vector3f& _vector) const;

        Vector3f multiplyDirection(const Vector3f& _direction) const;

        Matrix4 divide(const Matrix4& _other) const;

        Matrix4 inverse() const;

        Matrix4 transpose() const;

        bool isIdentity() const;

        Vector4f getColumn(const uint32_t _index) const;

        Vector4f getRow(const uint32_t _index) const;

        void setColumn(const uint32_t _index, const Vector4f& _col);

        void setRow(const uint32_t _index, const Vector4f& _row);

        Vector3f getTranslation() const;

        Quaternion getRotation() const;

        Vector3f getScale() const;

        bool validTRS() const;

        void setTRS(const Vector3f& _pos, const Quaternion& _qua, const Vector3f& _scale);

        static Matrix4 Frustum(const float _left, const float _right,
                               const float _bottom, const float _top,
                               const float _zNear, const float _zFar);

        static Matrix4 Perspective(const float _fov, const float _aspect, const float _zNear, const float _zFar);

        static Matrix4 LookAt(const Vector3f& _from, const Vector3f& _to, const Vector3f& _up);

        static Matrix4 ViewMatrix(const Vector3f& _eye, const Vector3f& _center, const Vector3f& _up = Vector3f::Up);

        static Matrix4 Ortho(const float _left, const float _right, const float _bottom, const float _top,
                             const float _zNear, const float _zFar);

        static Matrix4 Translate(const Vector3f& _vector);

        static Matrix4 Rotate(const Quaternion& _qua);

        static Matrix4 Scale(const Vector3f& _scale);

        static Matrix4 TRS(const Vector3f& _pos, const Quaternion& _qua, const Vector3f& _scale);

        template<typename _T1, typename _T2>
        static bool Decompose(const Matrix4& _model, Vector3<_T1>& _pos, Quaternion& _rotation, Vector3<_T2>& _scale);

        std::string toString() const;

        bool operator==(const Matrix4& _other) const;

        bool operator!=(const Matrix4& _other) const {
            return !(*this == _other);
        }

        Matrix4 operator-() const;

        Matrix4 operator+(const Matrix4& _right) const;

        Matrix4 operator-(const Matrix4& _right) const;

        Matrix4 operator*(const Matrix4& _right) const;

        Matrix4 operator/(const Matrix4& _right) const;

        Matrix4 operator*(const float _right) const;

        Matrix4 operator/(const float _right) const;

        Vector4f operator*(const Vector4f& _right) const;

        const Vector4f& operator[](const uint32_t _index) const;

        Vector4f& operator[](const uint32_t _index) {
            return const_cast<Vector4f&>(static_cast<const Matrix4&>(*this)[_index]);
        }

        Matrix4& operator+=(const Matrix4& _right);

        Matrix4& operator-=(const Matrix4& _right);

        Matrix4& operator*=(const Matrix4& _right);

        Matrix4& operator/=(const Matrix4& _right);

        Matrix4& operator*=(const float& _right);

        Matrix4& operator/=(const float& _right);
    };

    template <typename _T1, typename _T2>
    bool Matrix4::Decompose(const Matrix4& _model, Vector3<_T1>& _pos, Quaternion& _rotation, Vector3<_T2>& _scale) {
        glm::vec3 skew;
        glm::vec4 perspective;
        return glm::decompose(_model.mat, _scale.vec, _rotation.qua, _pos.vec, skew, perspective);
    }

    Matrix4 operator*(const float _left, const Matrix4& _right);

}

#endif

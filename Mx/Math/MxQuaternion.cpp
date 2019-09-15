#include "MxMatrix4.h"
#include "MxQuaternion.h"
#include "MxMatrix4.h"
#include "MxMath.h"

#include "boost/format.hpp"
#include <glm/gtx/quaternion.hpp>

namespace Mix {
    Quaternion::Quaternion(const float _w, const float _x, const float _y, const float _z) :
        w(_w), x(_x), y(_y), z(_z) {
    }

    Quaternion::Quaternion(const Vector4f& _source) :
        w(_source.w), x(_source.x), y(_source.y), z(_source.z) {
    }

    Quaternion::Quaternion(const float _source[4]) :
        w(_source[0]), x(_source[1]), y(_source[2]), z(_source[3]) {
    }

    Quaternion::Quaternion(const Matrix4& _source) {
        float tr = _source[0][0] + _source[1][1] + _source[2][2];

        if (tr > 0.0f) {
            float w4 = std::sqrt(1.0f + tr)*2.0f;
            w = w4 / 4.0f;
            x = (_source[2][1] - _source[1][2]) / w4;
            y = (_source[0][2] - _source[2][0]) / w4;
            z = (_source[1][0] - _source[0][1]) / w4;
        }
        else if (_source[0][0] > _source[1][1] && _source[0][0] > _source[2][2]) {
            float x4 = std::sqrt(1.0f + _source[0][0] - _source[1][1] - _source[2][2]) * 2.0f;
            w = (_source[2][1] - _source[1][2]) / x4;
            x = x4 / 4.0f;
            y = (_source[0][1] + _source[1][0]) / x4;
            z = (_source[0][2] + _source[2][0]) / x4;
        }
        else if (_source[1][1] > _source[2][2]) {
            float y4 = std::sqrt(1.0f + _source[1][1] - _source[0][0] - _source[2][2]) * 2.0f;
            w = (_source[0][2] - _source[2][0]) / y4;
            x = (_source[0][1] + _source[1][0]) / y4;
            y = y4 / 4.0f;
            z = (_source[1][2] + _source[2][1]) / y4;
        }
        else {
            float z4 = std::sqrt(1.0f + _source[2][2] - _source[0][0] - _source[1][1]) * 2.0f;
            w = (_source[1][0] - _source[0][1]) / z4;
            x = (_source[0][2] + _source[2][0]) / z4;
            y = (_source[1][2] + _source[2][1]) / z4;
            z = z4 / 4.0f;
        }
    }

    Quaternion Quaternion::multiply(const Quaternion& _other) const {
        return Quaternion(w * _other.w - x * _other.x - y * _other.y - z * _other.z,
                          w * _other.x + x * _other.w + y * _other.z - z * _other.y,
                          w * _other.y + y * _other.w + z * _other.x - x * _other.z,
                          w * _other.z + z * _other.w + x * _other.y - y * _other.x);
    }

    Vector3f Quaternion::multiply(const Vector3f& _point) const {
        return Vector3f(qua * _point.vec);
    }

    float Quaternion::dot(const Quaternion& _other) const {
        return w * _other.w + x * _other.x + y * _other.y + z * _other.z;
    }

    Quaternion Quaternion::lerp(const Quaternion& _other, const float _step) const {
        assert(_step >= 0.0f);
        assert(_step <= 1.0f);

        return (*this * (1.0f - _step) + _other * _step).normalize();
    }

    Quaternion Quaternion::slerp(const Quaternion& _other, const float _step) const {
        Quaternion qua = _other;

        float cosTheta = this->dot(_other);

        // If cosTheta < 0
        if (cosTheta < 0.0f) {
            qua = -_other;
            cosTheta = -cosTheta;
        }

        if (cosTheta > 1 - Math::Constants::Epsilon) {
            // Linear interpolation
            return Quaternion(w * (1 - _step) + qua.w * _step,
                              x * (1 - _step) + qua.x * _step,
                              y * (1 - _step) + qua.y * _step,
                              z * (1 - _step) + qua.z * _step);
        }

        const float angle = std::acos(cosTheta);
        return (this->scale(std::sin((1.0f - _step) * angle)) + qua.scale(std::sin(_step * angle))) / std::sin(angle);
    }

    Quaternion Quaternion::inverse() const {
#ifdef MX_QUATERNION_MORE_OPERATION
        return Quaternion(w, -x, -y, -z) / dot(*this);
#endif
        return Quaternion(w, -x, -y, -z);
    }

    Quaternion Quaternion::normalize() const {
        const float l = length();

        if (!(l > 0.0f)) {
            throw Exception("Can't normalize a zero length quaternion");
        }

        return Quaternion(w / l, x / l, y / l, z / l);
    }

    Matrix4 Quaternion::toMatrix() const {
        Matrix4 result(1.0f);

        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;
        const float xz = x * z;
        const float xy = x * y;
        const float yz = y * z;
        const float wx = w * x;
        const float wy = w * y;
        const float wz = w * z;

        result[0][0] = 1.0f - 2.0f * (yy + zz);
        result[0][1] = 2.0f * (xy + wz);
        result[0][2] = 2.0f * (xz - wy);

        result[1][0] = 2.0f * (xy - wz);
        result[1][1] = 1.0f - 2.0f * (xx + zz);
        result[1][2] = 2.0f * (yz + wx);

        result[2][0] = 2.0f * (xz + wy);
        result[2][1] = 2.0f * (yz - wx);
        result[2][2] = 1.0f - 2.0f * (xx + yy);

        return result;
    }

    Vector3f Quaternion::toEuler() const {
        Vector3f result;
        result.vec = glm::eulerAngles(qua);
        return result;
    }

    void Quaternion::toAxisAngle(float& _angle, Vector3f& _axis) const {
        _angle = getAngle();
        _axis = getAxis();
    }

    float Quaternion::getAngle() const {
        return std::acos(w) * 2.0f;
    }

    Vector3f Quaternion::getAxis() const {
        float s = 1.0f - w * w; // sin^2(theta/2)

        if (s <= 0.0f) // theta is 180 degree, return Up as default axis
            return Vector3f::Up;

        float sqrts = std::sqrt(s);
        return Vector3f(x / sqrts, y / sqrts, z / sqrts);
    }

    const float& Quaternion::operator[](const uint32_t _index) const {
        switch (_index) {
        case 0:
            return w;
        case 1:
            return x;
        case 2:
            return y;
        case 3:
            return z;
        default:
            throw Exception("Quaternion index out of bounds!");
        }
    }

    float Quaternion::length() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    Quaternion Quaternion::scale(const float _other) const {
        return Quaternion(w*_other, x*_other, y*_other, z*_other);
    }

    Quaternion Quaternion::add(const Quaternion& _other) const {
        return Quaternion(w + _other.w, x + _other.x, y + _other.y, z + _other.z);
    }

    Quaternion Quaternion::minus(const Quaternion& _other) const {
        return Quaternion(w - _other.w, x - _other.x, y - _other.y, z - _other.z);
    }

    Quaternion Quaternion::operator+(const Quaternion& _right) const {
        return add(_right);
    }

    Quaternion Quaternion::operator-(const Quaternion& _right) const {
        return minus(_right);
    }

    Quaternion Quaternion::operator/(const Quaternion& _right) const {
        return Quaternion(w / _right.w, x / _right.x, y / _right.y, z / _right.z);
    }

    Quaternion Quaternion::operator*(const float _a) const {
        return scale(_a);
    }

    Quaternion Quaternion::operator/(const float _a) const {
        return Quaternion(w / _a, x / _a, y / _a, z / _a);
    }

    Quaternion& Quaternion::operator*=(const float _a) {
        return *this = scale(_a);
    }

    Quaternion Quaternion::operator*(const Quaternion& _right) const {
        return multiply(_right);
    }

    Quaternion& Quaternion::operator*=(const Quaternion& _other) {
        return *this = multiply(_other);
    }

    Vector3f Quaternion::operator*(const Vector3f& _point) const {
        return multiply(_point);
    }

    std::string Quaternion::toString() const {
        return (boost::format("(%d, %d, %d, %d)") % w % x % y % z).str();
    }

    Quaternion Quaternion::Euler(const float _xAngle, const float _yAngle, const float _zAngle) {
        return AngleAxis(_xAngle, Vector3f::Right) *
            AngleAxis(_yAngle, Vector3f::Up) *
            AngleAxis(_zAngle, Vector3f::Forward);
    }

    Quaternion Quaternion::Euler(const Vector3f& _euler) {
        return Euler(_euler.x, _euler.y, _euler.z);
    }

    Quaternion Quaternion::FromToRotation(const Vector3f& _fromDir, const Vector3f& _toDir) {
        auto norm1 = _fromDir.normalize();
        auto norm2 = _toDir.normalize();
        return Quaternion(glm::rotation(norm1.vec, norm2.vec));
    }

    Quaternion Quaternion::LookRotation(const Vector3f& _forward, const Vector3f& _up) {
        auto norm1 = _forward.normalize();
        auto norm2 = _up.normalize();
        return Quaternion(glm::quatLookAtLH(norm1.vec, norm2.vec));
    }

    Quaternion Quaternion::AngleAxis(const float _angle, const Vector3f& _axis) {
        Quaternion result;
        result.w = std::cos(_angle / 2.0f);
        float s = std::sin(_angle / 2.0f);
        auto norm = _axis.normalize();
        result.x = norm.x * s;
        result.y = norm.y * s;
        result.z = norm.z * s;
        return result;
    }

#ifdef MX_QUATERNION_MORE_OPERATION
    Quaternion operator*(const float _a, const Quaternion& _q) {
        return _q.scale(_a);
    }
#endif // MX_QUATERNION_MORE_OPERATION


    const Quaternion Quaternion::Identity = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

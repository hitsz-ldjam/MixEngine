#include "MxAABB.h"

namespace Mix {

    const AABB AABB::Unit = AABB();

    const AABB AABB::Infinity = AABB(
        Vector3f(
        -std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity()
    ),
        Vector3f(
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity()
    )
    );

    AABB::AABB() :mMin(-0.5f, -0.5, -0.5f), mMax(0.5f, 0.5f, 0.5f) {
    }

    AABB::AABB(const Vector3f& _min, const Vector3f& _max) : mMin(_min), mMax(_max) {
    }

    Vector3f AABB::getCorner(Corner _corner) const {
        switch (_corner) {
        case FarLeftBottom:     return Vector3f(mMin.x, mMin.y, mMax.z);
        case FarRightBottom:    return Vector3f(mMax.x, mMin.y, mMax.z);;
        case FarRightTop:       return Vector3f(mMax.x, mMax.y, mMax.z);
        case FarLeftTop:        return Vector3f(mMin.x, mMax.y, mMax.z);
        case NearLeftBottom:    return Vector3f(mMin.x, mMin.y, mMin.z);
        case NearRightBottom:   return Vector3f(mMax.x, mMin.y, mMin.z);
        case NearRightTop:      return Vector3f(mMax.x, mMax.y, mMin.z);
        case NearLeftTop:       return Vector3f(mMin.x, mMax.y, mMin.z);
        default:return Vector3f::Zero;
        }
    }

    AABB& AABB::translate(const Vector3f& _translate) {
        mMin += _translate;
        mMax += _translate;
        return *this;
    }

    AABB& AABB::scale(const Vector3f& _scale) {
        auto center = getCenter();
        auto offsetMin = mMin - center;
        auto offsetMax = mMax - center;

        offsetMin *= _scale;
        offsetMax *= _scale;

        mMin = offsetMin + center;
        mMax = offsetMax + center;
        return *this;
    }

    bool AABB::intersects(const AABB& _other) const {
        return !(
            (mMax.x < _other.mMin.x) ||
            (mMax.y < _other.mMin.y) ||
            (mMax.z < _other.mMin.z) ||
            (mMin.x > _other.mMax.x) ||
            (mMin.x > _other.mMax.x) ||
            (mMin.z > _other.mMax.z)
            );
    }

    Vector3f AABB::getCenter() const {
        return (mMin + mMax)*0.5f;
    }

    Vector3f AABB::getExtent() const {
        return mMax - mMin;
    }

    Vector3f AABB::getHalfExtent() const {
        return (mMax - mMin)*0.5f;
    }

    bool AABB::contains(const Vector3f& _point) const {
        return mMin.x <= _point.x && _point.x <= mMax.x &&
            mMin.y <= _point.y && _point.y <= mMax.y &&
            mMin.z <= _point.z && _point.z <= mMax.z;
    }

    bool AABB::contains(const AABB& _other) const {
        return mMin.x <= _other.mMin.x &&
            mMin.y <= _other.mMin.y &&
            mMin.z <= _other.mMin.z &&
            _other.mMax.x <= mMax.x &&
            _other.mMax.y <= mMax.y &&
            _other.mMax.z <= mMax.z;
    }

    bool AABB::operator==(const AABB& _other) const {
        return mMin == _other.mMin && mMax == _other.mMax;
    }


}

#include "MxPlane.h"
#include "MxMatrix4.h"
#include "MxAABB.h"

namespace Mix {
    Plane::Plane(const Vector3f& _normal, float _d)
        :normal(_normal), d(_d) {
    }

    Plane::Plane(const Vector3f& _normal, const Vector3f& _point)
        : normal(_normal), d(_normal.dot(_point)) {
    }

    Plane::Plane(const Vector3f& _point0, const Vector3f& _point1, const Vector3f& _point2) {
        Vector3f edge1 = _point1 - _point0;
        Vector3f edge2 = _point2 - _point0;
        normal = edge1.cross(edge2);
        normal.normalize();
        d = normal.dot(_point0);
    }

    float Plane::getDistance(const Vector3f& _point) const {
        return normal.dot(_point) - d;
    }

    Plane::Side Plane::getSide(const Vector3f& _point, float _epsilon) const {
        float dist = getDistance(_point);

        if (dist > _epsilon)
            return PositiveSide;

        if (dist < -_epsilon)
            return NegativeSide;

        return NoSide;
    }

    Plane::Side Plane::getSide(const AABB& _box) const {
        float dist = getDistance(_box.getCenter());

        Vector3f halfSize = _box.getHalfExtent();
        float maxAbsDist = std::abs(normal.x * halfSize.x) + std::abs(normal.y * halfSize.y) + std::abs(normal.z * halfSize.z);

        if (dist < -maxAbsDist)
            return NegativeSide;

        if (dist > +maxAbsDist)
            return PositiveSide;

        return Plane::BothSide;
    }

    Vector3f Plane::projectVector(const Vector3f& point) const {
        Matrix4 xform;
        xform[0][0] = 1.0f - normal.x * normal.x;
        xform[1][0] = -normal.x * normal.y;
        xform[2][0] = -normal.x * normal.z;
        xform[0][1] = -normal.y * normal.x;
        xform[1][1] = 1.0f - normal.y * normal.y;
        xform[2][1] = -normal.y * normal.z;
        xform[0][2] = -normal.z * normal.x;
        xform[1][2] = -normal.z * normal.y;
        xform[2][2] = 1.0f - normal.z * normal.z;
        return xform.multiplyDirection(point);

    }

    float Plane::normalize() {
        float fLength = normal.length();

        if (fLength > 1e-08f) {
            float fInvLength = 1.0f / fLength;
            normal *= fInvLength;
            d *= fInvLength;
        }

        return fLength;
    }

    bool Plane::operator==(const Plane& _rhs) const {
        return (_rhs.d == d && _rhs.normal == normal);
    }

    bool Plane::operator!=(const Plane& _rhs) const {
        return (_rhs.d != d || _rhs.normal != normal);
    }
}

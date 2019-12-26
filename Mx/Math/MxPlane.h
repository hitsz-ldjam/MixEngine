#pragma once
#ifndef MX_PLANE_H_
#define MX_PLANE_H_
#include "MxVector3.h"

namespace Mix {
    class AABB;

    class Plane {
    public:
        enum Side {
            NoSide,
            PositiveSide,
            NegativeSide,
            BothSide
        };

    public:
        Plane() = default;

        Plane(const Plane& _other) = default;

        Plane(const Vector3f& _normal, float _d);

        Plane(const Vector3f& _normal, const Vector3f& _point);

        Plane(const Vector3f& _point0, const Vector3f& _point1, const Vector3f& _point2);

        Plane& operator= (const Plane& _other) = default;

        Side getSide(const Vector3f& _point, float _epsilon = 0.0f) const;

        Side getSide(const AABB& _box) const;

        float getDistance(const Vector3f& _point) const;

        Vector3f projectVector(const Vector3f& _v) const;

        float normalize();

        bool operator==(const Plane& _rhs) const;

        bool operator!=(const Plane& _rhs) const;

    public:
        Vector3f normal;
        float d = 0.0f;

    };
}
#endif

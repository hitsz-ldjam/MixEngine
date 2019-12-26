#pragma once
#ifndef MX_FRUSTUM_H_
#define MX_FRUSTUM_H_
#include "MxMatrix4.h"
#include "MxPlane.h"

namespace Mix {
    enum class FrustumPlane {
        Near,
        Far,
        Left,
        Right,
        Top,
        Bottom
    };

    class Frustum {
    public:
        Frustum(const Matrix4& _projMat, bool _useNearPlanes = false);

        Frustum(const Frustum& _other) = default;

        Frustum& operator=(const Frustum& _other) = default;

        bool intersects(const AABB& _box) const;

        bool contains(const Vector3f& _point) const;

        const Plane& getPlane(FrustumPlane _plane) const;

    private:
        Plane mPlanes[6];
    };
}

#endif

#include "MxFrustum.h"
#include "MxAABB.h"

namespace Mix {
    Frustum::Frustum(const Matrix4& _projMat, bool _useNearPlanes) {
        const Matrix4& proj = _projMat;

        // Left
        {
            Plane plane;
            plane.normal.x = proj[0][3] + proj[0][0];
            plane.normal.y = proj[1][3] + proj[1][0];
            plane.normal.z = proj[2][3] + proj[2][0];
            plane.d = proj[3][3] + proj[3][0];

            mPlanes[uint32(FrustumPlane::Left)] = plane;
        }

        // Right
        {
            Plane plane;
            plane.normal.x = proj[0][3] - proj[0][0];
            plane.normal.y = proj[1][3] - proj[1][0];
            plane.normal.z = proj[2][3] - proj[2][0];
            plane.d = proj[3][3] - proj[3][0];

            mPlanes[uint32(FrustumPlane::Right)] = plane;
        }

        // Top
        {
            Plane plane;
            plane.normal.x = proj[0][3] - proj[0][1];
            plane.normal.y = proj[1][3] - proj[1][1];
            plane.normal.z = proj[2][3] - proj[2][1];
            plane.d = proj[3][3] - proj[3][1];

            mPlanes[uint32(FrustumPlane::Top)] = plane;

        }

        // Bottom
        {
            Plane plane;
            plane.normal.x = proj[0][3] + proj[0][1];
            plane.normal.y = proj[1][3] + proj[1][1];
            plane.normal.z = proj[2][3] + proj[2][1];
            plane.d = proj[3][3] + proj[3][1];

            mPlanes[uint32(FrustumPlane::Bottom)] = plane;

        }

        // Far
        {
            Plane plane;
            plane.normal.x = proj[0][3] - proj[0][2];
            plane.normal.y = proj[1][3] - proj[1][2];
            plane.normal.z = proj[2][3] - proj[2][2];
            plane.d = proj[3][3] - proj[3][2];

            mPlanes[uint32(FrustumPlane::Far)] = plane;

        }

        // Near
        if (_useNearPlanes) {
            Plane plane;
            plane.normal.x = proj[0][3] + proj[0][2];
            plane.normal.y = proj[1][3] + proj[1][2];
            plane.normal.z = proj[2][3] + proj[2][2];
            plane.d = proj[3][3] + proj[3][2];

            mPlanes[uint32(FrustumPlane::Near)] = plane;

        }

        for (uint32 i = 0; i < 6; i++) {
            float length = mPlanes[i].normal.length();
            if (length != 0.0f) {
                mPlanes[i].d /= -length;
                mPlanes[i].normal = mPlanes[i].normal.normalize();
            }
        }
    }

    bool Frustum::intersects(const AABB& _box) const {
        Vector3f center = _box.getCenter();
        Vector3f extents = _box.getHalfExtent();
        Vector3f absExtents(std::abs(extents.x), std::abs(extents.y), std::abs(extents.z));

        for (auto& plane : mPlanes) {
            float dist = center.dot(plane.normal) - plane.d;

            float effectiveRadius = absExtents.x * std::abs(plane.normal.x);
            effectiveRadius += absExtents.y * std::abs(plane.normal.y);
            effectiveRadius += absExtents.z * std::abs(plane.normal.z);

            if (dist < -effectiveRadius)
                return false;
        }

        return true;
    }

    bool Frustum::contains(const Vector3f& _point) const {
        for (auto& plane : mPlanes) {
            if (plane.getDistance(_point) < 0.0f)
                return false;
        }

        return true;
    }

    const Plane& Frustum::getPlane(FrustumPlane _plane) const {
        return mPlanes[uint32(_plane)];
    }
}

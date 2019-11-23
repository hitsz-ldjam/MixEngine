#pragma once
#ifndef MX_AABB_H_
#define MX_AABB_H_
#include "MxVector3.h"
#include "MxMatrix4.h"

namespace Mix {

    class AABB {
    public:

        /*
                 ^+y
                 |
                 |     / +z
              2--|------1 ->max
             /|  |   / /|
            / |  |  / / |
           /  |  | / /  |
          6---------5   |
     -----|------|--|--------->
          |   3-/|--|---0    +x
          |  / / |  |  /
          | / /  |  | /
          |/ /   |  |/
    min<- 7-/-------4
           /     |
                 |

         */


        enum Corner {
            FarLeftBottom = 3,
            FarRightBottom = 0,
            FarRightTop = 1,
            FarLeftTop = 2,

            NearLeftBottom = 7,
            NearRightBottom = 4,
            NearRightTop = 5,
            NearLeftTop = 6,

        };

        AABB();

        AABB(const Vector3f& _min, const Vector3f& _max);

        const Vector3f& getMin() const { return mMin; }

        const Vector3f& getMax() const { return mMax; }

        void setMin(const Vector3f& _min) { mMin = _min; }

        void setMax(const Vector3f& _max) { mMax = _max; }

        Vector3f getCorner(Corner _corner) const;

        AABB& translate(const Vector3f& _translate);

        AABB& scale(const Vector3f& _scale);

        bool intersects(const AABB& _other) const;

        Vector3f getCenter() const;

        Vector3f getExtent() const;

        bool contains(const Vector3f& _point) const;

        bool contains(const AABB& _other) const;

        bool operator==(const AABB& _other) const;

        bool operator!=(const AABB& _other) const { return !(*this == _other); }

        static const AABB Unit;
        static const AABB Infinity;

    private:
        Vector3f mMin;
        Vector3f mMax;
    };
}

#endif

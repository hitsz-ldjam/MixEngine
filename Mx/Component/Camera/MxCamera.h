#pragma once
#ifndef MX_CAMERA_H_
#define MX_CAMERA_H_
#include "../../GameObject/MxGameObject.h"
#include "../../Math/MxVector.h"
#include "../../Math/MxFrustum.h"

namespace Mix {
    class Camera :public Behaviour {
        MX_DECLARE_RTTI;
    public:
        Camera(const Vector2i& _extent,
               float _fov = 45.0f);

        Matrix4 getViewMat() const;

        Matrix4 getProjMat() const;

        float getFov() const { return mFov; }

        void setFov(float _fov);

        float getNear() const { return mNear; }

        void setNear(float _near);

        float getFar() const {return mFar;}

        void setFar(float _far);

        Vector2i getExtent() const { return mExtent; }

        void setExtent(const Vector2i& _extent);

        Frustum getFrustum() const;

        Vector3f worldToScreenPoint(const Vector3f& _point) const;

    private:
        mutable bool mDirty;
        Vector2i mExtent;
        float mFov;
        float mNear = 0.1f;
        float mFar = 100.0f;

        mutable Matrix4 mProjMat;
        void updateMatrix() const;

    };
}

#endif

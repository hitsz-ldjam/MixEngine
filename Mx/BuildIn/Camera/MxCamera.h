#pragma once
#ifndef MX_CAMERA_H_
#define MX_CAMERA_H_
#include "../../GameObject/MxGameObject.h"
#include "../../Math/MxVector.h"

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

        Vector2i getExtent() const { return mExtent; }

        void setExtent(const Vector2i& _extent);

        Vector3f worldToScreenPoint(const Vector3f& _point) const;

    private:
        mutable bool mDirty;
        Vector2i mExtent;
        float mFov;

        mutable Matrix4 mProjMat;
        mutable Matrix4 mViewMat;
        void updateMatrix() const;

    };
}

#endif

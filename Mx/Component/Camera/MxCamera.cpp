#include "MxCamera.h"
#include "../../Input/MxInput.h"
#include "../../Window/MxWindow.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(Camera, Behaviour);

    Camera::Camera(const Vector2i& _extent,
                   float _fov) :mExtent(_extent), mDirty(true) {
        mFov = _fov;
        setFov(_fov);
    }

    Matrix4 Camera::getViewMat() const {
        return Matrix4::ViewMatrix(mGameObject->transform().getPosition(), mGameObject->transform().forward() + mGameObject->transform().getPosition(), mGameObject->transform().up());
    }

    Matrix4 Camera::getProjMat() const {
        updateMatrix();
        return mProjMat;
    }

    void Camera::setFov(float _fov) {
        if (10.0f < _fov && _fov < 170.0f) {
            mFov = _fov;
            mDirty = true;
        }
    }

    void Camera::setNear(float _near) {
        if (0.0f < _near && _near < mFar) {
            mNear = _near;
            mDirty = true;
        }
    }

    void Camera::setFar(float _far) {
        if (mNear < _far) {
            mFar = _far;
            mDirty = true;
        }
    }

    void Camera::setExtent(const Vector2i& _extent) {
        if (_extent.x > 0 && _extent.y > 0) {
            mExtent = _extent;
            mDirty = true;
        }
    }

    Frustum Camera::getFrustum() const {
        return Frustum(getProjMat() * getViewMat());
    }

    Vector3f Camera::worldToScreenPoint(const Vector3f& _point) const {
        updateMatrix();
        auto clipSpacePos = mProjMat * getViewMat() * Vector4f(_point, 1.0f);
        if (clipSpacePos.w == 0.0f)
            return Vector3f(_point.x, _point.y, 0.0f);
        auto ndcSpacePos = Vector3f(clipSpacePos) / clipSpacePos.w;
        auto windowPos = (Vector2f(ndcSpacePos) + 1.0f) / 2.0f * mExtent;
        return Vector3f(windowPos.x, windowPos.y, ndcSpacePos.z);
    }

    void Camera::updateMatrix() const {
        if (mDirty) {
            mProjMat = Matrix4::Perspective(Math::Radians(mFov), float(mExtent.x) / mExtent.y, mNear, mFar);
            mDirty = false;
        }
    }
}

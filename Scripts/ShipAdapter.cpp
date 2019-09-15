#include "ShipAdapter.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/BuildIn/Camera/MxCamera.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/Window/MxWindow.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(ShipAdapter, Script);

    void ShipAdapter::move(const Vector2f& _dir) {
        auto moveDir = _dir.length() > 0.0f ? _dir.normalize() : Vector2f::Zero;

        mSmoothMove = Vector2f::Lerp(mSmoothMove, moveDir * mMoveSpeed, mAccelerate);
        mSmoothMove = Vector2f::Lerp(mSmoothMove, Vector2f::Zero, mDecelerate);
        transform()->translate(mSmoothMove.x, mSmoothMove.y, 0.0f, Space::World);

        int hDir = moveDir.x > 0.0f ? -1 : (moveDir.x < 0.0f ? 1 : 0);
        mSmoothRollAngle = Math::Lerp(mSmoothRollAngle, mMaxRollAngle*hDir, mRoolAccelerate);
        transform()->setLocalRotation(Quaternion::AngleAxis(Radians(mSmoothRollAngle), Vector3f::Up));
    }

    void ShipAdapter::attack() {

        Log::Info("Fuck you");
    }

    void ShipAdapter::setMaxSpeed(float _speed) {
        mMoveSpeed = _speed;
    }

    void ShipAdapter::attackSpeed(float _speed) {
        if (_speed >= 0.0f)
            mAttackSpeed = _speed;
    }

    void ShipAdapter::init() {
    }

    void ShipAdapter::update() {
    }

    void ShipAdapter::fixedUpdate() {
    }
}
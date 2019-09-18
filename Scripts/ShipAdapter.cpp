#include "ShipAdapter.h"
#include "../Mx/Component/Camera/MxCamera.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/Window/MxWindow.h"
#include "ShipWeapon.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(ShipAdapter, Script);

    void ShipAdapter::move(const Vector2f& _dir) {
        auto moveDir = _dir.length() > 0.0f ? _dir.normalize() : Vector2f::Zero;

        mSmoothMove = Vector2f::Lerp(mSmoothMove, moveDir * mMoveSpeed, mAccelerate);
        mSmoothMove = Vector2f::Lerp(mSmoothMove, Vector2f::Zero, mDecelerate);
        transform()->translate(mSmoothMove.x, mSmoothMove.y, 0.0f, Space::World);

        int hDir = moveDir.x > 0.0f ? -1 : (moveDir.x < 0.0f ? 1 : 0);
        mSmoothRollAngle = Math::Lerp(mSmoothRollAngle, mMaxRollAngle*hDir, mRoolAccelerate);
        transform()->setLocalRotation(Quaternion::AngleAxis(Math::Radians(mSmoothRollAngle), Vector3f::Up));
    }

    void ShipAdapter::attack() {
        auto weapon = mGameObject->getComponent<ShipWeapon>();
        if (weapon != nullptr)
            weapon->attack();
    }

    void ShipAdapter::setMoveSpeed(float _speed) {
        mMoveSpeed = _speed;
    }

    void ShipAdapter::awake() {
    }

    void ShipAdapter::init() {
    }

    void ShipAdapter::update() {
    }

    void ShipAdapter::fixedUpdate() {
    }
}

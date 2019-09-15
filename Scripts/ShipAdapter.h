#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/Math/MxVector.h"
#include "../Mx/Utils/MxEvent.h"

namespace Scripts {
    using namespace Mix;
    using namespace Math;

    class ShipAdapter :public Script {
        MX_DECLARE_RTTI;
    public:
        ~ShipAdapter() = default;

        void move(const Vector2f& _dir);

        void attack();

        void setMaxSpeed(float _speed);

        void attackSpeed(float _speed);

    private:
        void init() override;

        void update() override;

        void fixedUpdate() override;

        // Move
        float mMoveSpeed = 0.2f;
        float mSlowMoveSpeedFactor = 0.5f;
        float mAccelerate = 0.5f;
        float mDecelerate = 0.1f;
        Vector2f mSmoothMove;

        // Roll
        float mSmoothRollAngle = 0.0f;
        float mRoolAccelerate = 0.2f;
        float mMaxRollAngle = 30.0f;

        // Attack
        float mLastAttackTime = 0.0f;
        float mAttackSpeed = 1.0f;
    };
}
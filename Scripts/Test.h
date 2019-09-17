#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/Math/MxVector.h"
#include "../Mx/Utils/MxEvent.h"

namespace Scripts {
	using namespace Mix;
	using namespace Math;

	class TestScript :public Mix::Script {
	public:
		~TestScript() = default;

	private:
		GameObject* mPlayerShip = nullptr;
		GameObject* mEnemyShip = nullptr;

		void init() override;

		void update() override;

		void fixedUpdate() override;

		Event<void()> mShutDownEvent;

		Vector2f mLookDir;
		Vector2f mSmoothMouse;
		Vector2f mSmoothMove;
		float mSmoothHeight = 0.0f;

		float mMouseSpeed = 8.0f;
		float mStickSpeed = 50.0f;

		float mMaxMoveSpeed = 15.0f;

		bool mUseCamepad = false;

		Vector3f mTarget;
		bool mLockOn = false;
		void lookAtTarget(const Vector3f& _target);
		void lockOn(const Vector3f& _target);
		void lockOff();
		void updateTargetLock();

		void useGamepad();
		void useKeyboard();
	};
}

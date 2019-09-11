#include "Test.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/Component/Transform/MxTransform.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/BuildIn/Camera/MxCamera.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/GUI/MxUi.h"
#include "../Mx/Time/MxTime.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/Physics/MxPhysicsWorld.h"
#include "../Mx/Component/RigidBody/MxRigidBody.h"
#include "../MixEngine.h"
#include <iostream>


namespace Scripts {
	void TestScript::init() {
		mPlayerShip = GameObject::Find("PlayerShip");
		mEnemyShip = GameObject::Find("EnemyShip");
		mShutDownEvent.connect(std::bind(&MixEngine::shutDown, &MixEngine::Instance()));
	}

	void TestScript::update() {
		if (Input::Get()->isButtonDown(ButtonCode::U))
			mUseCamepad = !mUseCamepad;

        if(!mUseCamepad && Input::Get()->isButtonDown(ButtonCode::Z))
            Physics::World::Get()->theWorld();
	}

	void TestScript::fixedUpdate() {
		if (mUseCamepad)
			useGamepad();
		else
			useKeyboard();
		updateTargetLock();

        if(auto hit = Physics::World::Get()->raycast({.0f, 15.0f, .0f}, {.0f, -1.f, .0f}, 15)) {
            auto& name = hit.rigidbody->getGameObject()->getName();
            std::cout << "hit on: " << name << ' ' << hit.point << std::endl;
        }
	}

	void TestScript::lookAtTarget(const Vector3f& _target) {
		auto equal = Math::EpsilonEqual(transform().getPosition(), _target);
		if (equal.x && equal.y && equal.z)
			return;

		auto dir = (_target - transform().getPosition()).normalize();
		Vector3f localDir;
		if (mGameObject->parent())
			localDir = mGameObject->parent()->transform().worldToLocalMatrix().multiplyDirection(dir);
		else
			localDir = dir;
		auto xz = Vector2f(localDir.x, localDir.z).normalize();
		mLookDir = { xz.y > 0.0f ? Degrees(std::asinf(xz.x)) : 180.0f - Degrees(std::asinf(xz.x)), Degrees(std::asinf(localDir.y)) };
	}

	void TestScript::lockOn(const Vector3f& _target) {
		mLockOn = true;
		mTarget = _target;
	}

	void TestScript::lockOff() {
		mLockOn = false;
	}

	void TestScript::updateTargetLock() {
		if (mLockOn)
			lookAtTarget(mTarget);
	}

	void TestScript::useGamepad() {
		// Update position
		auto dir = Vector2f(Input::Get()->getGamepadLeftStickAxis());
		auto h = Input::Get()->getAxis(AxisCode::Gamepad_RightTrigger) - Input::Get()->getAxis(AxisCode::Gamepad_LeftTrigger);

		if (dir.length() > 0.0f) {
			dir = dir.normalize() * mMaxMoveSpeed * Time::FixedDeltaTime();
			mSmoothMove = Vector2f::Lerp(mSmoothMove, Vector2f(dir), 0.8f);
		}

		mSmoothMove = Vector2f::Lerp(mSmoothMove, Vector2f::Zero, 0.1f);
		transform().translate(mSmoothMove.x, 0.0f, mSmoothMove.y);

		mSmoothHeight = Lerp(mSmoothHeight, h, 0.8f);
		mSmoothHeight = Lerp(mSmoothHeight, 0.0f, 0.1f);
		transform().translate(0.0f, mSmoothHeight, 0.0f, Space::World);

		if (Input::Get()->isButtonHold(ButtonCode::Gamepad_LB)) {
			mPlayerShip->transform().rotate(Vector3f::Up, Radians(60.0f * Time::FixedDeltaTime()));
		}
		if (Input::Get()->isButtonHold(ButtonCode::Gamepad_RB)) {
			mPlayerShip->transform().rotate(Vector3f::Up, Radians(-60.0f * Time::FixedDeltaTime()));
		}
		if (Input::Get()->isButtonDown(ButtonCode::Gamepad_X)) {
			static bool lockFlag = false;
			if (lockFlag)
				lockOn(mPlayerShip->transform().getPosition());
			else
				lockOff();
			lockFlag = !lockFlag;
		}
		if (Input::Get()->isButtonDown(ButtonCode::Gamepad_SpecialLeft)) {
			mShutDownEvent.trigger();
		}

		// Update direction
		Vector2f delta = Input::Get()->getGamepadRightStickAxis() * mStickSpeed * Time::FixedDeltaTime();
		mSmoothMouse = Vector2f::Lerp(mSmoothMouse, delta, 0.8f);
		mLookDir += mSmoothMouse;

		transform().setLocalRotation(Quaternion::AngleAxis(Radians(mLookDir.x), Vector3f::Up) *
									 Quaternion::AngleAxis(Radians(-mLookDir.y), Vector3f::Right));
	}

	void TestScript::useKeyboard() {
		// Update direction
		Vector2f delta;
		if (Input::Get()->isButtonHold(ButtonCode::Mouse_Right)) {
			delta = Vector2f(Input::Get()->getMousePositionDelta())*mMouseSpeed;
			delta.y = -delta.y;
		}
		delta = delta * Time::FixedDeltaTime();
		mSmoothMouse = Vector2f::Lerp(mSmoothMouse, delta, 0.8f);
		mLookDir += mSmoothMouse;

		transform().setLocalRotation(Quaternion::AngleAxis(Radians(mLookDir.x), Vector3f::Up) *
									 Quaternion::AngleAxis(Radians(-mLookDir.y), Vector3f::Right));

			  // Update position
		Vector2f dir;
		float h = 0.0f;
		if (Input::Get()->isButtonHold(ButtonCode::W)) {
			dir += Vector2f::Up;
		}
		if (Input::Get()->isButtonHold(ButtonCode::S)) {
			dir += Vector2f::Down;
		}
		if (Input::Get()->isButtonHold(ButtonCode::D)) {
			dir += Vector2f::Right;
		}
		if (Input::Get()->isButtonHold(ButtonCode::A)) {
			dir += Vector2f::Left;
		}
		if (Input::Get()->isButtonHold(ButtonCode::Space)) {
			h += .4f;
		}
		if (Input::Get()->isButtonHold(ButtonCode::LCtrl)) {
			h -= .4f;
		}
		if (Input::Get()->isButtonDown(ButtonCode::Escape)) {
			mShutDownEvent.trigger();
		}


		if (dir.length() > 0.0f) {
			dir = dir.normalize() * mMaxMoveSpeed * Time::FixedDeltaTime();
			mSmoothMove = Vector2f::Lerp(mSmoothMove, dir, 0.8f);
		}

		mSmoothMove = Vector2f::Lerp(mSmoothMove, Vector2f::Zero, 0.1f);
		transform().translate(mSmoothMove.x, 0.0f, mSmoothMove.y);

		mSmoothHeight = Lerp(mSmoothHeight, h, 0.8f);
		mSmoothHeight = Lerp(mSmoothHeight, 0.0f, 0.1f);
		transform().translate(0.0f, mSmoothHeight, 0.0f, Space::World);

		if (Input::Get()->isButtonHold(ButtonCode::E)) {
			mPlayerShip->transform().rotate(Vector3f::Up, Radians(60.0f * Time::FixedDeltaTime()));
		}
		if (Input::Get()->isButtonHold(ButtonCode::Q)) {
			mPlayerShip->transform().rotate(Vector3f::Up, Radians(-60.0f * Time::FixedDeltaTime()));
		}
		if (Input::Get()->isButtonDown(ButtonCode::X)) {
			static bool lockFlag = false;
			if (lockFlag)
				lockOn(mPlayerShip->transform().getPosition());
			else
				lockOff();
			lockFlag = !lockFlag;
		}
	}
}


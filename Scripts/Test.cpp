#include "Test.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/Component/Transform/MxTransform.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/BuildIn/Camera/MxCamera.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/GUI/MxUi.h"
#include "../Mx/Time/MxTime.h"
#include "../MixEngine.h"
#include <iostream>
#include "../Mx/Input/MxInputModule.h"


namespace Scripts {
	void TestScript::init() {
		mModel = GameObject::Find("Model0");
		mShutDownEvent.connect(std::bind(&MixEngine::shutDown, &MixEngine::Instance()));
	}

	void TestScript::update() {
		if (Input::Get()->isButtonDown(ButtonCode::U))
			mUseCamepad = !mUseCamepad;
		if (Input::Get()->isButtonDown(ButtonCode::Escape)) {
			mShutDownEvent.trigger();
		}
	}

	void TestScript::updateMouse() {
		auto tran = mGameObject->getComponent<Transform>();
		Vector2f delta;
		if (mUseCamepad) {
			delta = Input::Get()->getGamepadRightStickAxis()*mStickSpeed;
		}
		else {
			if (Input::Get()->isButtonHold(ButtonCode::Mouse_Right)) {
				delta = Vector2f(Input::Get()->getMousePositionDelta())*mMouseSpeed;
				delta.y = -delta.y;
			}
		}
		delta = delta * Time::FixedDeltaTime();
		mSmoothMouse = Vector2f::Lerp(mSmoothMouse, delta, 0.8f);
		mMouseLook += mSmoothMouse;

		tran->setLocalRotation(Quaternion::AngleAxis(Radians(mMouseLook.x), Vector3f::Up) *
							   Quaternion::AngleAxis(Radians(-mMouseLook.y), Vector3f::Right));

	}

	void TestScript::updatePos() {
		auto tran = mGameObject->getComponent<Transform>();

		Vector2f hDir;
		Vector3f vDir;
		if (mUseCamepad) {
			hDir = Vector2f(Input::Get()->getGamepadLeftStickAxis());
		}
		else {
			if (Input::Get()->isButtonHold(ButtonCode::W)) {
				hDir += Vector2f::Up;
			}
			if (Input::Get()->isButtonHold(ButtonCode::S)) {
				hDir += Vector2f::Down;
			}
			if (Input::Get()->isButtonHold(ButtonCode::D)) {
				hDir += Vector2f::Right;
			}
			if (Input::Get()->isButtonHold(ButtonCode::A)) {
				hDir += Vector2f::Left;
			}
			if (Input::Get()->isButtonHold(ButtonCode::Space)) {
				vDir += tran->worldToLocalMatrix().multiplyDirection(Vector3f::Up);
			}
			if (Input::Get()->isButtonHold(ButtonCode::LCtrl)) {
				vDir += tran->worldToLocalMatrix().multiplyDirection(Vector3f::Down);
			}
		}

		if (hDir.length() > 0.0f) {
			hDir = hDir.normalize();
			hDir *= mMaxMoveSpeed * Time::FixedDeltaTime();
			mSmoothMove = Vector3f::Lerp(mSmoothMove, Vector3f(hDir.x, 0.0f, hDir.y) + vDir * mMaxMoveSpeed, 0.8f);
		}

		mSmoothMove = Vector3f::Lerp(mSmoothMove, Vector3f::Zero, 0.1f);

		tran->translate(mSmoothMove);
	}

	void TestScript::fixedUpdate() {
		if (mUseCamepad) {
			if (Input::Get()->isButtonHold(ButtonCode::Gamepad_LB)) {
				mModel->transform().rotate(Vector3f::Up, Radians(60.0f * Time::FixedDeltaTime()));
			}
			if (Input::Get()->isButtonHold(ButtonCode::Gamepad_RB)) {
				mModel->transform().rotate(Vector3f::Up, Radians(-60.0f * Time::FixedDeltaTime()));
			}
		}
		else {
			if (Input::Get()->isButtonHold(ButtonCode::E)) {
				mModel->transform().rotate(Vector3f::Up, Radians(60.0f * Time::FixedDeltaTime()));
			}
			if (Input::Get()->isButtonHold(ButtonCode::Q)) {
				mModel->transform().rotate(Vector3f::Up, Radians(-60.0f * Time::FixedDeltaTime()));
			}
		}

		updateMouse();
		updatePos();
	}

	void TestScript::lookAtTarget(const Vector3f& _target) {
		// todo
		std::cout << "Not implement yet!" << std::endl;
	}
}


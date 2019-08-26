#include "Test.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/Component/Transform/MxTransform.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/BuildIn/Camera/MxCamera.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/GUI/MxUi.h"
#include "../Mx/Time/MxTime.h"
#include <iostream>


namespace Scripts {
	void TestScript::init() {
		mModel = GameObject::Find("Model0");
	}

	void TestScript::update() {
		if (Input::GetButtonDown(SDL_SCANCODE_F)) {
			lookAtTarget(mModel->transform().getPosition());
		}
	}

	void TestScript::updateMouse() {
		if (Input::MouseButtonState() & SDL_BUTTON_RMASK) {
			auto tran = mGameObject->getComponent<Transform>();

			Vector2f delta = Vector2f(Input::MousePositionDelta());
			delta = delta * mMouseSpeed * Time::FixedDeltaTime();
			delta.y = -delta.y;
			mSmoothMouse = Vector2f::Lerp(mSmoothMouse, delta, 0.8f);
			mMouseLook += mSmoothMouse;

			tran->setLocalRotation(Quaternion::AngleAxis(Radians(mMouseLook.x), Vector3f::Up) *
								   Quaternion::AngleAxis(Radians(-mMouseLook.y), Vector3f::Right));
		}
	}

	void TestScript::updatePos() {
		auto tran = mGameObject->getComponent<Transform>();

		Vector3f dir;

		if (Input::GetAxisRaw(SDL_SCANCODE_W)) {
			dir += Vector3f::Forward;
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_S)) {
			dir += Vector3f::Back;
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_D)) {
			dir += Vector3f::Right;
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_A)) {
			dir += Vector3f::Left;
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_SPACE)) {
			dir += tran->worldToLocalMatrix().multiplyDirection(Vector3f::Up);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_LCTRL)) {
			dir += tran->worldToLocalMatrix().multiplyDirection(Vector3f::Down);
		}

		if (dir.length() > 0.0f) {
			dir = dir.normalize();
			dir *= mMaxMoveSpeed * Time::FixedDeltaTime();
			mSmoothMove = Vector3f::Lerp(mSmoothMove, dir, 0.8f);
		}

		mSmoothMove = Vector3f::Lerp(mSmoothMove, Vector3f::Zero, 0.1f);

		tran->translate(mSmoothMove);
	}

	void TestScript::fixedUpdate() {
		if (Input::GetAxisRaw(SDL_SCANCODE_E)) {
			mModel->transform().rotate(Vector3f::Up, Radians(60.0f * Time::FixedDeltaTime()));
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_Q)) {
			mModel->transform().rotate(Vector3f::Up, Radians(-60.0f * Time::FixedDeltaTime()));
		}

		updateMouse();
		updatePos();
	}

	void TestScript::lookAtTarget(const Vector3f& _target) {
		// todo
		std::cout << "Not implement yet!" << std::endl;
	}
}


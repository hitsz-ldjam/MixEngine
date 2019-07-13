#include "MxCamera.h"
#include "../../Input/MxInput.h"
#include "../../Time/MxTime.h"
#include "../../Log/MxLog.h"
#include "../../../MixEngine.h"
#include "../../Window/MxWindow.h"


namespace Mix {
	using namespace Math;

	void Camera::awake() {
		mGameObject->transform().setPosition(mPos);
		mGameObject->transform().lookAt(mTarget);
	}

	void Camera::update() {
		auto tran = mGameObject->getComponent<Transform>();

		if (Input::MouseButtonState() & SDL_BUTTON_RMASK) {
			auto delta = Input::MousePositionDelta();
			tran->rotate(Radians(mMouseSpeed * delta.y * Time::DeltaTime()), Radians(mMouseSpeed * delta.x * Time::DeltaTime()), 0.0f);
		}

		if (Input::GetAxisRaw(SDL_SCANCODE_W)) {
			tran->translate(Vector3f::Forward * (Time::DeltaTime() * 5.0f), Space::SELF);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_S)) {
			tran->translate(Vector3f::Back * (Time::DeltaTime() * 5.0f), Space::SELF);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_D)) {
			tran->translate(Vector3f::Right * (Time::DeltaTime() * 5.0f), Space::SELF);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_A)) {
			tran->translate(Vector3f::Left * (Time::DeltaTime() * 5.0f), Space::SELF);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_SPACE)) {
			tran->translate(Vector3f::Up * (Time::DeltaTime() * 5.0f), Space::WORLD);
		}
		if (Input::GetAxisRaw(SDL_SCANCODE_LCTRL)) {
			tran->translate(Vector3f::Down * (Time::DeltaTime() * 5.0f), Space::WORLD);
		}

		if (Input::GetButtonDown(SDL_SCANCODE_Q)) {
			tran->lookAt(mTarget);
			//tran->rotate(Vector3f::Up, Radians(Time::DeltaTime()*-45.0f));
			//Debug::Log::Info("Q");
		}

		//tran->lookAt(mTarget);
	}

	Camera::Camera(const Math::Vector3f& _pos,
				   const Math::Vector3f& _target,
				   const Math::Vector2i& _extent,
				   float _mouseSpeed,
				   float _fov) :mExtent(_extent) {
		mPos = _pos;

		mTarget = Vector3f::Forward;
		if (_target != _pos)
			mTarget = _target;

		mFov = _fov;
		setFov(_fov);

		mMouseSpeed = _mouseSpeed;
		setMouseSpeed(_mouseSpeed);
	}

	void Camera::setTarget(const Vector3f& _target) {
		if (_target != mGameObject->transform().getPosition())
			mTarget = _target;
	}

	const Vector3f& Camera::getTarget() const {
		return mTarget;
	}

	Matrix4 Camera::getViewMat() const {
		return Matrix4::ViewMatrix(mGameObject->transform().getPosition(), mGameObject->transform().localToWorldMatrix().multiplyDirection(Vector3f::Forward) + mGameObject->transform().getPosition(), Vector3f::Up);
	}

	Matrix4 Camera::getProjMat() const {
		Matrix4 proj = Matrix4::Perspective(Radians(mFov),
											float(mExtent.x) / mExtent.y,
											0.1f, 1000.0f);
		proj[1][1] *= -1.0f;
		return proj;
	}

	void Camera::setFov(float _fov) {
		if (10.0f < _fov && _fov < 170.0f)
			mFov = _fov;
	}

	void Camera::setMouseSpeed(float _mouseSpeed) {
		if (0.0f < _mouseSpeed)
			mMouseSpeed = _mouseSpeed;
	}

	void Camera::setExtent(const Math::Vector2i& _extent) {
		if (_extent.x > 0 && _extent.y > 0)
			mExtent = _extent;
	}
}

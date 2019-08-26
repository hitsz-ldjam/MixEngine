#include "MxCamera.h"
#include "../../Input/MxInput.h"
#include "../../Time/MxTime.h"
#include "../../Log/MxLog.h"
#include "../../../MixEngine.h"
#include "../../Window/MxWindow.h"

namespace Mix {
	using namespace Math;

	Camera::Camera(const Math::Vector2i& _extent,
				   float _fov) :mExtent(_extent) {
		mFov = _fov;
		setFov(_fov);
	}

	Matrix4 Camera::getViewMat() const {
		return Matrix4::ViewMatrix(mGameObject->transform().getPosition(), mGameObject->transform().forward() + mGameObject->transform().getPosition(), Vector3f::Up);
	}

	Matrix4 Camera::getProjMat() const {
		Matrix4 proj = Matrix4::Perspective(Radians(mFov), float(mExtent.x) / mExtent.y, 0.1f, 1000.0f);
		proj[1][1] *= -1.0f;
		return proj;
	}

	void Camera::setFov(float _fov) {
		if (10.0f < _fov && _fov < 170.0f)
			mFov = _fov;
	}

	void Camera::setExtent(const Math::Vector2i& _extent) {
		if (_extent.x > 0 && _extent.y > 0)
			mExtent = _extent;
	}
}

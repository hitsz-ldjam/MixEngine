#pragma once
#ifndef MX_CAMERA_H_
#define MX_CAMERA_H_
#include "../../GameObject/MxGameObject.h"
#include "../../Math/MxVector.h"

namespace Mix {
	class Camera :public Behaviour {
		void awake() override;
		void update() override;

	public:
		Camera(const Math::Vector3f& _pos, const Math::Vector3f& _target, const Math::Vector2i& _extent, float _mouseSpeed = 10.0f, float _fov = 45.0f);

		void setTarget(const Math::Vector3f& _target);

		const Math::Vector3f& getTarget() const;

		Math::Matrix4 getViewMat() const;

		Math::Matrix4 getProjMat() const;

		float getFov() const { return mFov; }

		void setFov(float _fov);

		float getMouseSpeed() const { return mMouseSpeed; }

		void setMouseSpeed(float _mouseSpeed);

		Math::Vector2i getExtent() const { return mExtent; }

		void setExtent(const Math::Vector2i& _extent);

	private:
		Math::Vector3f mPos;
		Math::Vector3f mTarget;
		Math::Vector2i mExtent;
		float mFov;
		float mMouseSpeed;
	};
}

#endif

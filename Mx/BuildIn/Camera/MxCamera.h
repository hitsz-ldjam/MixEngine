#pragma once
#ifndef MX_CAMERA_H_
#define MX_CAMERA_H_
#include "../../GameObject/MxGameObject.h"
#include "../../Math/MxVector.h"

namespace Mix {
	class Camera :public Behaviour {
	public:
		Camera(const Math::Vector2i& _extent,
			   float _fov = 45.0f);

		Math::Matrix4 getViewMat() const;

		Math::Matrix4 getProjMat() const;

		float getFov() const { return mFov; }

		void setFov(float _fov);

		Math::Vector2i getExtent() const { return mExtent; }

		void setExtent(const Math::Vector2i& _extent);

	private:
		Math::Vector3f mTarget;
		Math::Vector2i mExtent;
		float mFov;
	};
}

#endif

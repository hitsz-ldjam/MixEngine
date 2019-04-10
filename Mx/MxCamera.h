#pragma once
#ifndef _MX_CAMERA_H_
#define _MX_CAMERA_H_

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "MxGameObject.h"
#include"MxTransform.h"

//#include"MxUtils.h"


namespace Mix
{
	class Camera : public Component
	{
	public:
		enum CameraDirection { FORWARD, BACK, LEFT, RIGHT, UP, DOWN, WORLD_UP, WORLD_DOWN };

	private:

		Transform* ptransform;

        float mFovy;
        float mAspect;
        float mNear;
        float mFar;

		float mMoveSpeed;
		float mMouseSensitivity;
	public:
		Camera();
        Camera(const float fovy, const float aspect, const float near, const float far);

		// get view matrix used in vulkan
		glm::mat4 getViewMatrix() const;

		void moveCamera(const float deltaTime, glm::vec3 direction, bool normalized = true);
		void moveCamera(const float deltaTime, const CameraDirection direction);

		void rotateCamera(const float yawOffset, const float pitchOffset, const float rollOffset, const float deltaTime);

		void setPosition(const glm::vec3& pos);
		void setPosition(const float x, const float y, const float z) { setPosition(glm::vec3(x, y, z)); };
		glm::vec3 getPosition() const { return ptransform->position(); };

		void setMoveSpeed(const float speed);
		float getMoveSpeed() const { return mMoveSpeed; };

		void lookAt(const glm::vec3& target);
		void lookAt(const float x, const float y, const float z);

		void setMouseSensitivity(const float sensitivity);
		float getMouseSensitivity() const { return mMouseSensitivity; };

        void setPerspective(const float fovy, const float aspect, const float near, const float far);
        glm::mat4 getProjMat() const;

		const Transform* getTransform() const { return ptransform; };
	};

}

#endif
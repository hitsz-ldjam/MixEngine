#pragma once
#ifndef _MX_CAMERA_H_
#define _MX_CAMERA_H_

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"MxUtils.h"

namespace Mix
{
	class MxCamera
	{
	public:
		enum CameraDirection { FORWARD, BACK, LEFT, RIGHT, UP, DOWN, WORLD_UP, WORLD_DOWN };

	private:
		MxTransform mTransform;

		float mMoveSpeed;
		float mMouseSensitivity;
	public:
		MxCamera();

		// get view matrix used in vulkan
		glm::mat4 getViewMatrix() const;

		void moveCamera(const float deltaTime, glm::vec3 direction, bool normalized = true);
		void moveCamera(const float deltaTime, const CameraDirection direction);

		void rotateCamera(const float yawOffset, const float pitchOffset, const float rollOffset);

		void setPosition(const glm::vec3& pos);
		void setPosition(const float x, const float y, const float z) { setPosition(glm::vec3(x, y, z)); };
		glm::vec3 getPosition() const { return mTransform.getPosition(); };

		void setMoveSpeed(const float speed);
		float getMoveSpeed() const { return mMoveSpeed; };

		void lookAt(const glm::vec3& target);
		void lookAt(const float x, const float y, const float z);

		void setMouseSensitivity(const float sensitivity);
		float getMouseSensitivity() const { return mMouseSensitivity; };

		const MxTransform getTransform() const { return mTransform; };
	};

}

#endif
#include "MxCamera.h"

namespace Mix
{
	MxCamera::MxCamera() :mTransform(), mMoveSpeed(1.0f), mMouseSensitivity(1.0f)
	{
	}

	glm::mat4 MxCamera::getViewMatrix() const
	{
		return glm::lookAt(mTransform.getPosition(),
						   mTransform.getPosition() + mTransform.getForward(),
						   mTransform.getUp());
	}

	void MxCamera::moveCamera(const float deltaTime, glm::vec3 direction, bool normalized)
	{
		if (!normalized)
			direction = glm::normalize(direction);

		mTransform.setPosition(mTransform.getPosition() + direction * deltaTime * mMoveSpeed);
	}

	void MxCamera::moveCamera(const float deltaTime, const CameraDirection direction)
	{
		glm::vec3 dir;

		switch (direction)
		{
		case Mix::MxCamera::FORWARD:
			dir = mTransform.getForward();
			break;
		case Mix::MxCamera::BACK:
			dir = -mTransform.getForward();
			break;
		case Mix::MxCamera::LEFT:
			dir = -mTransform.getRight();
			break;
		case Mix::MxCamera::RIGHT:
			dir = mTransform.getRight();
			break;
		case Mix::MxCamera::UP:
			dir = mTransform.getUp();
			break;
		case Mix::MxCamera::DOWN:
			dir = -mTransform.getUp();
			break;
		case Mix::MxCamera::WORLD_UP:
			dir = MxAxis::initZ;
			break;
		case Mix::MxCamera::WORLD_DOWN:
			dir = -MxAxis::initZ;
			break;
		}

		moveCamera(deltaTime, dir);
	}

	void MxCamera::rotateCamera(const float yawOffset, const float pitchOffset, const float rollOffset)
	{
		mTransform.rotate(rollOffset, pitchOffset, yawOffset);
	}

	void MxCamera::setPosition(const glm::vec3& pos)
	{
		mTransform.setPosition(pos);
	}

	void MxCamera::setMoveSpeed(const float speed)
	{
		if (speed < 0.0f)
			return;
		mMoveSpeed = speed;
	}

	void MxCamera::lookAt(const glm::vec3& target)
	{
		mTransform.lookAt(target);
	}

	void MxCamera::lookAt(const float x, const float y, const float z)
	{
		lookAt(glm::vec3(x, y, z));
	}

	void MxCamera::setMouseSensitivity(const float sensitivity)
	{
		if (sensitivity < 0.0f)
			return;
		mMouseSensitivity = sensitivity;
	}

}

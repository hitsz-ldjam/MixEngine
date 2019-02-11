#include "MxUtils.h"

namespace Mix
{
	const glm::vec3 MxAxis::initX = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 MxAxis::initY = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 MxAxis::initZ = glm::vec3(0.0f, 0.0f, 1.0f);

	MxAxis::MxAxis() :
		x(initX),
		y(initY),
		z(initZ)
	{
	}

	void MxAxis::applyRotateMat(const glm::mat4 & matrix)
	{
		glm::mat3 mat = matrix;
		x = mat * x;
		y = mat * y;
		z = mat * z;
	}

	void MxAxis::rotateFromInit(const glm::mat4 & matrix)
	{
		glm::mat3 mat = matrix;
		x = mat * initX;
		y = mat * initY;
		z = mat * initZ;
	}

	void MxTransform::quaternionUpdated()
	{
		mAxis.rotateFromInit(glm::toMat4(mQuaternion));
	}

	MxTransform::MxTransform() :
		mPosition(0.0f, 0.0f, 0.0f),
		mScale(1.0f, 1.0f, 1.0f),
		mAxis(),
		mQuaternion(1.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	glm::vec3 MxTransform::getEulerAngles() const
	{
		// to avoid negative zero
		return glm::eulerAngles(mQuaternion) + glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void MxTransform::translate(const glm::vec3 & translation, const MxSpace relativeTo)
	{
		switch (relativeTo)
		{
		case Mix::MxSpace::World:
			mPosition += translation;
			break;
		case Mix::MxSpace::Self:
			mPosition += (translation.x*mAxis.x + translation.y*mAxis.y + translation.z*mAxis.z);
			break;
		}
	}

	void MxTransform::translate(const float x, const float y, const float z, const MxSpace relativeTo)
	{
		translate(glm::vec3(x, y, z), relativeTo);
	}

	void MxTransform::rotate(const glm::vec3 & eulers, const MxSpace relativeTo)
	{
		switch (relativeTo)
		{
		case Mix::MxSpace::World:
			mQuaternion = glm::tquat(eulers) * mQuaternion;
			break;
		case Mix::MxSpace::Self:
			mQuaternion = glm::angleAxis(eulers.z, mAxis.z) *
				glm::angleAxis(eulers.y, mAxis.y) *
				glm::angleAxis(eulers.x, mAxis.x) * mQuaternion;
			break;
		}

		quaternionUpdated();
	}

	void MxTransform::rotateAround(const glm::vec3 & point, const glm::vec3 & axis, const float angle)
	{
		glm::vec3 relativePos = mPosition - point;
		glm::mat4 mat = glm::rotate(glm::mat4(1.0f), angle, axis);
		relativePos = mat * glm::vec4(relativePos, 1.0f);
		mPosition = relativePos + point;
	}

	void MxTransform::scale(const glm::vec3 & scale, const MxSpace relativeTo)
	{
		switch (relativeTo)
		{
		case Mix::MxSpace::World:
			mScale = scale;
			break;
		case Mix::MxSpace::Self:
			mScale *= scale;
			break;
		}
	}

	void MxTransform::lookAt(const MxTransform & target, const glm::vec3 & worldUp)
	{
		lookAt(target.getPosition(), worldUp);
	}

	void MxTransform::lookAt(const glm::vec3 & worldPosition, const glm::vec3 & worldUp)
	{
		// if worldPosition is equal to camera position
		// do nothing
		auto equal = glm::epsilonEqual(worldPosition, mPosition, MX_EPSILON);
		if (equal.x && equal.y && equal.z)
			return;

		glm::vec3 direction = glm::normalize(worldPosition - mPosition);
		mQuaternion = rotateBetweenVec(MxAxis::initX, direction);
		quaternionUpdated();

		// rotate when worldUp is perpendicular to direction
		// float theta = glm::dot(worldUp, direction);
		// if (glm::epsilonEqual(theta, 0.0f, MX_EPSILON))
		//	mQuaternion = rotateBetweenVec(mAxis.z, worldUp)*mQuaternion;

		// rotate when mAxis.y is not perpendicular to worldUp
		float theta = glm::dot(worldUp, mAxis.y);
		if (glm::epsilonNotEqual(theta, 0.0f, MX_EPSILON))
		{
			glm::vec3 y = glm::normalize(glm::cross(worldUp, mAxis.x));
			mQuaternion = rotateBetweenVec(mAxis.y, y) * mQuaternion;
			quaternionUpdated();
		}



	}

	glm::quat rotateBetweenVec(const glm::vec3 & from, const glm::vec3 & to)
	{
		float cosTheta = glm::dot(from, to);
		glm::vec3 rotationAxis;

		if (cosTheta > 1.0f - MX_EPSILON)
			return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		else if (cosTheta < -1.0f + MX_EPSILON)
		{
			rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), from);
			if (glm::length(rotationAxis) < MX_EPSILON) // they were parallel
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), from);

			rotationAxis = glm::normalize(rotationAxis);
			return glm::angleAxis(glm::radians(180.0f), rotationAxis);
		}

		rotationAxis = glm::normalize(glm::cross(from, to));

		return glm::angleAxis(glm::acos(cosTheta), rotationAxis);
	}

}
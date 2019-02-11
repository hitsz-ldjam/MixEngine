#pragma once
#ifndef _MX_UTILS_H_
#define _MX_UTILS_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/euler_angles.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/gtx/quaternion.hpp>

namespace Mix
{
	constexpr float MX_EPSILON = 0.000001f;
	constexpr float MX_PI = 3.141593f;

	//

	//MxSpace
	enum class MxSpace { World, Self };

	glm::quat rotateBetweenVec(const glm::vec3& from, const glm::vec3& to);

	struct MxAxis
	{
		glm::vec3 x;
		glm::vec3 y;
		glm::vec3 z;

		static const glm::vec3 initX;
		static const glm::vec3 initY;
		static const glm::vec3 initZ;

		MxAxis();
		void applyRotateMat(const glm::mat4& matrix);
		void rotateFromInit(const glm::mat4& matrix);
	};

	//MxTransform
	class MxTransform
	{
	private:
		glm::vec3 mPosition;
		glm::vec3 mScale;
		MxAxis mAxis;
		glm::quat mQuaternion;

		void quaternionUpdated();

	public:
		MxTransform();
		//set position in world coordinates
		void setPosition(const glm::vec3& position) { mPosition = position; };
		void setPosition(const float x, const float y, const float z) { setPosition(glm::vec3(x, y, z)); };
		glm::vec3 getPosition() const { return mPosition; };

		//get rotation in world coordinates stored as quaternion
		glm::quat getRotation() const { return mQuaternion; };

		//get rotation in world coordinates as euler angles
		glm::vec3 getEulerAngles() const;

		//get local coordinates
		const MxAxis& getAxis() const { return mAxis; };
		glm::vec3 getForward() const { return mAxis.x; };
		glm::vec3 getRight() const { return -mAxis.y; };
		glm::vec3 getUp() const { return mAxis.z; };

		void translate(const glm::vec3& translation, const MxSpace relativeTo = MxSpace::Self);
		void translate(const float x, const float y, const float z, const MxSpace relativeTo = MxSpace::Self);

		void rotate(const glm::vec3& eulers, const MxSpace relativeTo = MxSpace::Self);
		void rotate(const float xAngle,
					const float yAngle,
					const float zAngle,
					const MxSpace relativeTo = MxSpace::Self)
		{
			rotate(glm::vec3(xAngle, yAngle, zAngle), relativeTo);
		};
		void rotate(const glm::vec3& axis, const float angle, const MxSpace relativeTo = MxSpace::Self)
		{
			rotate(glm::eulerAngles(glm::angleAxis(angle, axis)), relativeTo);
		}

		//rotates about axis passing through point in world coordinates by angle radians
		void rotateAround(const glm::vec3& point, const glm::vec3& axis, const float angle);

		void scale(const glm::vec3& scale, const MxSpace relativeTo = MxSpace::Self);

		void lookAt(const glm::vec3& worldPosition, const glm::vec3& worldUp = MxAxis::initZ);
		void lookAt(const MxTransform& target, const glm::vec3& worldUp = MxAxis::initZ);
	};
}

#endif
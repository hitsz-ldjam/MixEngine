#include "MxTransform.h"
#include "../../Math/MxMath.h"

#include <glm/gtx/quaternion.hpp>

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(Transform, Component);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Transform);

    const glm::vec3 Axis::WorldX = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 Axis::WorldY = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 Axis::WorldZ = glm::vec3(0.0f, 0.0f, 1.0f);

    const glm::vec3 Axis::WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 Axis::WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 Axis::WorldForward = glm::vec3(0.0f, 0.0f, 1.0f);

    void Axis::applyRotate(const glm::mat4& _mat) {
        glm::mat3 m = _mat;
        x = m * x;
        y = m * y;
        z = m * z;
    }

    void Axis::applyRotate(const glm::quat& _quat) {
        x = _quat * x;
        y = _quat * y;
        z = _quat * z;
    }

    void Axis::rotateFromInit(const glm::mat4& _mat) {
        glm::mat3 m = _mat;
        x = m * WorldX;
        y = m * WorldY;
        z = m * WorldZ;
    }

    void Axis::rotateFromInit(const glm::quat& _quat) {
        x = _quat * WorldX;
        y = _quat * WorldY;
        z = _quat * WorldZ;
    }

    Transform::Transform()
        : mPosition(0.0f, 0.0f, 0.0f),
        mScale(1.0f, 1.0f, 1.0f),
        mQuaternion(1.0f, 0.0f, 0.0f, 0.0f) {
    }

    glm::vec3 Transform::eulerAngle() const {
        return glm::eulerAngles(mQuaternion) + glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Transform::translate(const glm::vec3& _translation, const Space _relativeTo) {
        switch (_relativeTo) {
        case Space::WORLD:
            mPosition += _translation;
            break;
        case Space::SELF:
            mPosition += (_translation.x * mAxis.x + _translation.y * mAxis.y + _translation.z * mAxis.z);
            break;
        }
    }

    void Transform::rotate(const glm::vec3& _eulers, const Space _relativeTo) {
        switch (_relativeTo) {
        case Space::WORLD:
            mQuaternion = glm::tquat<float>(_eulers) * mQuaternion;
            break;
        case Space::SELF:
            mQuaternion = glm::tquat<float>(glm::eulerAngles(mQuaternion) + _eulers);
            break;
        }
        quaternionUpcdated();
    }

    void Transform::rotateAround(const glm::vec3& _point, const glm::vec3& _axis, const float _angle) {
        glm::vec3 relativePos = mPosition - _point;
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), _angle, _axis);
        relativePos = mat * glm::vec4(relativePos, 1.0f);
        mPosition = relativePos + _point;
    }

    void Transform::scale(const glm::vec3& _scale, const Space _relativeTo) {
        switch (_relativeTo) {
        case Space::WORLD:
            mScale = _scale;
            break;
        case Space::SELF:
            mScale *= _scale;
            break;
        }
    }

    void Transform::lookAt(const glm::vec3& _worldPosition, const glm::vec3& _worldUp) {
        // if worldPosition is equal to camera position, do nothing
        const auto equal = glm::epsilonEqual(_worldPosition, mPosition, Math::Constants::Epsilon);
        if (equal.x && equal.y && equal.z)
            return;
        const glm::vec3 dir = glm::normalize(_worldPosition - mPosition);

        float theta = glm::dot(dir, forward());
        // if dir and forward is in same direction, do nothing
        if(glm::epsilonEqual(theta,1.0f,Math::Constants::Epsilon)) {
            return;
        }

        // if dir and forward is in opposite direction
        if(glm::epsilonEqual(theta, -1.0f, Math::Constants::Epsilon)) {
            mQuaternion = glm::angleAxis(glm::radians(180.0f), mAxis.y)*mQuaternion;
            return;
        }

        theta = glm::dot(dir, _worldUp);

        // if dir and worldUp is in the same direction
        if (glm::epsilonEqual(glm::abs(theta), 1.0f, Math::Constants::Epsilon)) {
            mQuaternion = glm::rotation(mAxis.z, dir) * mQuaternion;
            quaternionUpcdated();
            return;
        }

        mQuaternion = glm::quatLookAt(dir, _worldUp);
        quaternionUpcdated();
    }

    void Transform::quaternionUpcdated() {
        mAxis.rotateFromInit(mQuaternion);
    }
}

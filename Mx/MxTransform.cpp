#include "MxTransform.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NoCreateFunc(Transform, Component);
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Transform);

    const glm::vec3 Axis::worldX = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 Axis::worldY = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 Axis::worldZ = glm::vec3(0.0f, 0.0f, 1.0f);

    const glm::vec3 Axis::worldRight = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 Axis::worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 Axis::worldForward = glm::vec3(0.0f, 0.0f, 1.0f);

    void Axis::applyRotate(const glm::mat4 & mat) {
        glm::mat3 m = mat;
        x = m * x;
        y = m * y;
        z = m * z;
    }

    void Axis::applyRotate(const glm::quat & quat) {
        x = quat * x;
        y = quat * y;
        z = quat * z;
    }

    void Axis::rotateFromInit(const glm::mat4 & mat) {
        glm::mat3 m = mat;
        x = m * worldX;
        y = m * worldY;
        z = m * worldZ;
    }

    void Axis::rotateFromInit(const glm::quat & quat) {
        x = quat * worldX;
        y = quat * worldY;
        z = quat * worldZ;
    }

    Transform::Transform()
        :mPosition(0.0f, 0.0f, 0.0f),
        mScale(1.0f, 1.0f, 1.0f),
        mAxis(),
        mQuaternion(1.0f, 0.0f, 0.0f, 0.0f) {
    }

    Component * Transform::copy() const {
        Transform* t = new Transform(*this);
        t->mGameObj = nullptr;
        return t;
    }

    glm::vec3 Transform::eulerAngle() const {
        return glm::eulerAngles(mQuaternion) + glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Transform::translate(const glm::vec3 & translation, const Space relativeTo) {
        switch (relativeTo) {
        case Space::World:
            mPosition += translation;
            break;
        case Space::Self:
            mPosition += (translation.x * mAxis.x + translation.y * mAxis.y + translation.z * mAxis.z);
            break;
        }
    }

    void Transform::rotate(const glm::vec3 & eulers, const Space relativeTo) {
        switch (relativeTo) {
        case Space::World:
            mQuaternion = glm::tquat<float>(eulers)*mQuaternion;
            break;
        case Space::Self:
            mQuaternion = glm::tquat<float>(glm::eulerAngles(mQuaternion) + eulers);
            break;
        }
        quaternionUpcdated();
    }

    void Transform::rotateAround(const glm::vec3 & point, const glm::vec3 & axis, const float angle) {
        glm::vec3 relativePos = mPosition - point;
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), angle, axis);
        relativePos = mat * glm::vec4(relativePos, 1.0f);
        mPosition = relativePos + point;
    }

    void Transform::scale(const glm::vec3 & scale, const Space relativeTo) {
        switch (relativeTo) {
        case Space::World:
            mScale = scale;
            break;
        case Space::Self:
            mScale *= scale;
            break;
        }
    }

    void Transform::lookAt(const glm::vec3 & worldPosition, const glm::vec3 & worldUp) {
        // if worldPosition is equal to camera position, do nothing
        auto equal = glm::epsilonEqual(worldPosition, mPosition, Constants::Epsilon);
        if (equal.x && equal.y && equal.z)
            return;

        glm::vec3 dir = glm::normalize(worldPosition - mPosition);
        float theta = glm::dot(dir, worldUp);

        // if dir and worldUp is in the same direction
        if (glm::epsilonEqual(glm::abs(theta), 1.0f, Constants::Epsilon)) {
            mQuaternion = glm::rotation(mAxis.z, dir) * mQuaternion;
            quaternionUpcdated();
            return;
        }

        mQuaternion = glm::quatLookAt(dir, worldUp);
        quaternionUpcdated();
    }

    void Transform::quaternionUpcdated() {
        mAxis.rotateFromInit(mQuaternion);
    }
}

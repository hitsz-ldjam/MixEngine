#pragma once
#ifndef _MX_TRANSFORM_H_
#define _MX_TRANSFORM_H_

#include "MxComponent.h"

namespace Mix {
    struct Axis {
        union {
            struct {
                glm::vec3 x;
                glm::vec3 y;
                glm::vec3 z;
            };

            struct {
                glm::vec3 right;
                glm::vec3 up;
                glm::vec3 forward;
            };
        };

        //static const glm::vec3 worldX;
        static const glm::vec3 worldX;
        static const glm::vec3 worldY;
        static const glm::vec3 worldZ;

        static const glm::vec3 worldRight;
        static const glm::vec3 worldUp;
        static const glm::vec3 worldForward;

        Axis() :x(worldX), y(worldY), z(worldZ) {}

        void applyRotate(const glm::mat4& mat);
        void applyRotate(const glm::quat& quat);
        void rotateFromInit(const glm::mat4& mat);
        void rotateFromInit(const glm::quat& quat);
    };

    class Transform :public Component {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        Transform();

        const glm::vec3& position() const { return mPosition; }
        glm::vec3& position() { return mPosition; }
        const glm::quat& rotation() const { return mQuaternion; }
        glm::quat& rotation() { return mQuaternion; }
        const glm::vec3 scale() const { return mScale; }
        glm::vec3 scale() { return mScale; }

        glm::vec3 eulerAngle() const;

        const Axis& localAxis() const { return mAxis; }
        glm::vec3 forward() const { return mAxis.z; }
        glm::vec3 right() const { return mAxis.x; }
        glm::vec3 up() const { return mAxis.y; }

        void translate(const glm::vec3& translation, const Space relativeTo = Space::Self);
        void translate(const float x, const float y, const float z, const Space relativeTo = Space::Self) {
            translate(glm::vec3(x, y, z), relativeTo);
        }
        void translate(const glm::vec3& translation, const Transform& relativeTo) {
            mPosition = mPosition + translation.x * relativeTo.mAxis.x +
                translation.y * relativeTo.mAxis.y +
                translation.z * relativeTo.mAxis.z;
        }
        void translate(const float x, const float y, const float z, const Transform& relativeTo) {
            translate(glm::vec3(x, y, z), relativeTo);
        }

        void rotate(const glm::vec3& eulers, const Space relativeTo = Space::Self);
        void rotate(const float xAngle, const float yAngle, const float zAngle, const Space relativeTo = Space::Self) {
            rotate(glm::vec3(xAngle, yAngle, zAngle), relativeTo);
        };
        void rotate(const glm::vec3& axis, const float angle, const Space relativeTo = Space::Self) {
            rotate(glm::eulerAngles(glm::angleAxis(angle, axis)), relativeTo);
        }

        void rotateAround(const glm::vec3& point, const glm::vec3& axis, const float angle);

        void scale(const glm::vec3& scale, const Space relativeTo = Space::Self);

        void lookAt(const glm::vec3& worldPosition, const glm::vec3& worldUp = Axis::worldUp);
        void lookAt(const Transform& target, const glm::vec3& worldUp = Axis::worldUp) {
            lookAt(target.position(), worldUp);
        }

    private:
        glm::vec3 mPosition;
        glm::vec3 mScale;
        glm::quat mQuaternion;
        Axis mAxis;

        void quaternionUpcdated();
    };
}
#endif // !_MX_TRANSFORM_H_

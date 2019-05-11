#pragma once

#ifndef MX_TRANSFORM_H_
#define MX_TRANSFORM_H_

#include "../../Definitions/MxDefinitions.h"
#include "../MxComponent.h"

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

        static const glm::vec3 worldX;
        static const glm::vec3 worldY;
        static const glm::vec3 worldZ;

        static const glm::vec3 worldRight;
        static const glm::vec3 worldUp;
        static const glm::vec3 worldForward;

        Axis() : x(worldX),
                 y(worldY),
                 z(worldZ) {}

        void applyRotate(const glm::mat4& _mat);
        void applyRotate(const glm::quat& _quat);
        void rotateFromInit(const glm::mat4& _mat);
        void rotateFromInit(const glm::quat& _quat);
    };

    class Transform : public Component {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        Transform();

        const glm::vec3& position() const { return mPosition; }
        const glm::quat& rotation() const { return mQuaternion; }
        glm::vec3 eulerAngle() const;

        const Axis& localAxis() const { return mAxis; }
        glm::vec3 forward() const { return mAxis.z; }
        glm::vec3 right() const { return mAxis.x; }
        glm::vec3 up() const { return mAxis.y; }

        void translate(const glm::vec3& _translation, const Space _relativeTo = Space::SELF);

        void translate(const float _x, const float _y, const float _z, const Space _relativeTo = Space::SELF) {
            translate(glm::vec3(_x, _y, _z), _relativeTo);
        }

        void translate(const glm::vec3& _translation, const Transform& _relativeTo) {
            mPosition = mPosition + _translation.x * _relativeTo.mAxis.x +
                        _translation.y * _relativeTo.mAxis.y +
                        _translation.z * _relativeTo.mAxis.z;
        }

        void translate(const float _x, const float _y, const float _z, const Transform& _relativeTo) {
            translate(glm::vec3(_x, _y, _z), _relativeTo);
        }

        void rotate(const glm::vec3& _eulers, const Space _relativeTo = Space::SELF);

        void rotate(const float _xAngle, const float _yAngle, const float _zAngle, const Space _relativeTo = Space::SELF) {
            rotate(glm::vec3(_xAngle, _yAngle, _zAngle), _relativeTo);
        };

        void rotate(const glm::vec3& _axis, const float _angle, const Space _relativeTo = Space::SELF) {
            rotate(glm::eulerAngles(glm::angleAxis(_angle, _axis)), _relativeTo);
        }

        void rotateAround(const glm::vec3& _point, const glm::vec3& _axis, const float _angle);

        void scale(const glm::vec3& _scale, const Space _relativeTo = Space::SELF);

        void lookAt(const glm::vec3& _worldPosition, const glm::vec3& _worldUp = Axis::worldUp);

        void lookAt(const Transform& _target, const glm::vec3& _worldUp = Axis::worldUp) {
            lookAt(_target.position(), _worldUp);
        }

    private:
        glm::vec3 mPosition;
        glm::vec3 mScale;
        glm::quat mQuaternion;
        Axis mAxis;

        void quaternionUpcdated();
    };
}

#endif

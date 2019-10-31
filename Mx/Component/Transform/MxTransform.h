#pragma once

#ifndef MX_TRANSFORM_H_
#define MX_TRANSFORM_H_

#include "../../Definitions/MxCommonType.h"
#include "../MxComponent.h"

#include "../../Math/MxMatrix4.h"
#include "../../Math/MxQuaternion.h"

namespace Mix {
    class Transform :public Component {
        MX_DECLARE_RTTI;
    public:
        Vector3f getPosition() const; // done

        void setPosition(const Vector3f& _pos); // done

        Quaternion getRotation() const; // done

        void setRotation(const Quaternion& _qua); // done

        Vector3f getLossyScale() const; // done

        const Vector3f& getLocalPosition() const { return mPosition; } // done

        void setLocalPosition(const Vector3f& _pos) {
            mPosition = _pos;
            recurSetHasChanged();
        } // done

        const Quaternion& getLocalRotation() const { return mQuat; } // done

        void setLocalRotation(const Quaternion& _qua) {
            mQuat = _qua;
            recurSetHasChanged();
        } // done

        const Vector3f& getLocalScale() const { return mScale; } // done

        void setLocalScale(const Vector3f& _scale) {
            mScale = _scale;
            recurSetHasChanged();
        } // done

        bool hasChanged() const { return mChanged; } // done

        Vector3f forward() const; // done

        Vector3f right() const; // done

        Vector3f up() const; // done

        const Matrix4& localToWorldMatrix() const; // done

        Matrix4 worldToLocalMatrix() const; // done

        Transform* root() const;

        void translate(const Vector3f& _translation, const Space _relativeTo = Space::Self); // done

        void translate(const float _x, const float _y, const float _z, const Space _relativeTo = Space::Self) { //done
            translate(Vector3f(_x, _y, _z), _relativeTo);
        }

        void translate(const Vector3f& _translation, const Transform& _relativeTo); // done

        void translate(const float _x, const float _y, const float _z, const Transform& _relativeTo) { // done
            translate(Vector3f(_x, _y, _z), _relativeTo);
        }

        void rotate(const Vector3f& _eulers, const Space _relativeTo = Space::Self) { // done
            rotate(Quaternion::Euler(_eulers), _relativeTo);
        }

        void rotate(const float _xAngle, const float _yAngle, const float _zAngle, const Space _relativeTo = Space::Self) { // done
            rotate(Vector3f(_xAngle, _yAngle, _zAngle), _relativeTo);
        };

        void rotate(const Vector3f& _axis, const float _angle, const Space _relativeTo = Space::Self) { // done
            rotate(Quaternion::AngleAxis(_angle, _axis), _relativeTo);
        }

        void rotate(const Quaternion& _qua, const Space _relativeTo = Space::Self); // done

        void rotateAround(const Vector3f& _point, const Vector3f& _axis, const float _angle); // done

        void lookAt(const Vector3f& _worldPosition, const Vector3f& _worldUp = Vector3f::Up);

        void lookAt(const Transform& _target, const Vector3f& _worldUp = Vector3f::Up) { // done
            lookAt(_target.getPosition(), _worldUp);
        }

        void forceUpdate();

    private:
        Vector3f mPosition;
        Quaternion mQuat;
        Vector3f mScale = Vector3f::One;

        mutable Matrix4 mLocalToWorld;
        mutable bool mChanged = true;

        void updateLocalToWorldMatrix() const;

        void recurSetHasChanged() const;
    };
}

#endif

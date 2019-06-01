#pragma once

#ifndef MX_TRANSFORM_H_
#define MX_TRANSFORM_H_

#include "../../Definitions/MxDefinitions.h"
#include "../MxComponent.h"

#include "../../Math/MxMatrix4.h"
#include "../../Math/MxQuaternion.h"

namespace Mix {
    class Transform :public Component {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        Math::Vector3f getPosition() const; // done

        void setPosition(const Math::Vector3f& _pos); // done

        Math::Quaternion getRotation() const; // done

        void setRotation(const Math::Quaternion& _qua); // done

        Math::Vector3f getLossyScale() const; // done

        const Math::Vector3f& getLocalPosition() const { return mPosition; } // done

        void setLocalPosition(const Math::Vector3f& _pos) {
            mPosition = _pos;
            recurSetHasChanged();
        } // done

        const Math::Quaternion& getLocalRotation() const { return mQuat; } // done

        void setLocalRotation(const Math::Quaternion& _qua) {
            mQuat = _qua;
            recurSetHasChanged();
        } // done

        const Math::Vector3f& getLocalScale() const { return mScale; } // done

        void setLocalScale(const Math::Vector3f& _scale) {
            mScale = _scale;
            recurSetHasChanged();
        } // done

        bool hasChanged() const { return mChanged; } // done

        Math::Vector3f forward() const; // done

        Math::Vector3f right() const; // done

        Math::Vector3f up() const; // done

        const Math::Matrix4& localToWorldMatrix() const; // done

        Math::Matrix4 worldToLocalMatrix() const; // done

        Transform* root() const;

        void translate(const Math::Vector3f& _translation, const Space _relativeTo = Space::SELF); // done

        void translate(const float _x, const float _y, const float _z, const Space _relativeTo = Space::SELF) { //done
            translate(Math::Vector3f(_x, _y, _z), _relativeTo);
        }

        void translate(const Math::Vector3f& _translation, const Transform& _relativeTo); // done

        void translate(const float _x, const float _y, const float _z, const Transform& _relativeTo) { // done
            translate(Math::Vector3f(_x, _y, _z), _relativeTo);
        }

        void rotate(const Math::Vector3f& _eulers, const Space _relativeTo = Space::SELF) { // done
            rotate(Math::Quaternion::Euler(_eulers), _relativeTo);
        }

        void rotate(const float _xAngle, const float _yAngle, const float _zAngle, const Space _relativeTo = Space::SELF) { // done
            rotate(Math::Vector3f(_xAngle, _yAngle, _zAngle), _relativeTo);
        };

        void rotate(const Math::Vector3f& _axis, const float _angle, const Space _relativeTo = Space::SELF) { // done
            rotate(Math::Quaternion::AngleAxis(_angle, _axis), _relativeTo);
        }

        void rotate(const Math::Quaternion& _qua, const Space _relativeTo = Space::SELF); // done

        void rotateAround(const Math::Vector3f& _point, const Math::Vector3f& _axis, const float _angle); // done
        
        void lookAt(const Math::Vector3f& _worldPosition, const Math::Vector3f& _worldUp = Math::Vector3f::Up);

        void lookAt(const Transform& _target, const Math::Vector3f& _worldUp = Math::Vector3f::Up) { // done
            lookAt(_target.getPosition(), _worldUp);
        }

    private:
        Math::Vector3f mPosition;
        Math::Quaternion mQuat;
        Math::Vector3f mScale = Math::Vector3f::One;

        mutable Math::Matrix4 mLocalToWorld;
        mutable bool mChanged = true;

        void updateLocalToWorldMatrix() const;

        void recurSetHasChanged() const;
    };
}

#endif

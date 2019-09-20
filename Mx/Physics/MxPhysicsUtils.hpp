#pragma once

#ifndef MX_PHYSICS_UTILS_HPP_
#define MX_PHYSICS_UTILS_HPP_

#include "../Component/Transform/MxTransform.h"
#include <bullet3/LinearMath/btTransform.h>

namespace Mix::Physics {
    inline Transform& bt_mx_cast(const btTransform& _btTrans, Transform& _mxTrans) noexcept {
        _mxTrans.setPosition({
            -_btTrans.getOrigin().x(),
            _btTrans.getOrigin().y(),
            _btTrans.getOrigin().z()
        });
        _mxTrans.setRotation({
            -_btTrans.getRotation().w(),
            -_btTrans.getRotation().x(),
            _btTrans.getRotation().y(),
            _btTrans.getRotation().z()
        });
        return _mxTrans; // symmetric
    }

    inline Vector3f bt_mx_cast(const btVector3& _btVec) noexcept {
        return Vector3f(_btVec.x(), _btVec.y(), _btVec.z());
    }

    inline Quaternion bt_mx_cast(const btQuaternion& _btQuat) noexcept {
        return Quaternion(_btQuat.w(), _btQuat.x(), _btQuat.y(), _btQuat.z());
    }

    inline btTransform mx_bt_cast(const Transform& _mxTrans) noexcept {
        return btTransform({
                               -_mxTrans.getRotation().x,
                               _mxTrans.getRotation().y,
                               _mxTrans.getRotation().z,
                               -_mxTrans.getRotation().w
                           },
                           {
                               -_mxTrans.getPosition().x,
                               _mxTrans.getPosition().y,
                               _mxTrans.getPosition().z
                           });
    }

    inline btVector3 mx_bt_cast(const Vector3f& _mxVec) noexcept {
        return btVector3(_mxVec.x, _mxVec.y, _mxVec.z);
    }

    inline btQuaternion mx_bt_cast(const Quaternion& _mxQuat) noexcept {
        return btQuaternion(_mxQuat.x, _mxQuat.y, _mxQuat.z, _mxQuat.w);
    }
}

#endif

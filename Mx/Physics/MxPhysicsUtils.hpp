#pragma once

#ifndef MX_PHYSICS_UTILS_HPP_
#define MX_PHYSICS_UTILS_HPP_

#include "../Component/Transform/MxTransform.h"
#include <bullet3/LinearMath/btTransform.h>

namespace Mix::Physics {
    struct RaycastHit final {
        HRigidBody rigidbody;
        Vector3f hitpoint;

        explicit RaycastHit(HRigidBody _rb, const Vector3f& _p) : rigidbody(std::move(_rb)),
                                                                  hitpoint(_p) {}

        explicit operator bool() const { return rigidbody; }
    };

    inline Vector3f bt_mx_cast(const btVector3& _btVec, const bool _switchHandness = true) noexcept {
        if(_switchHandness)
            return Vector3f(_btVec.x(), _btVec.y(), -_btVec.z());
        return Vector3f(_btVec.x(), _btVec.y(), _btVec.z());
    }

    inline Quaternion bt_mx_cast(const btQuaternion& _btQuat, const bool _switchHandness = true) noexcept {
        if(_switchHandness)
            return Quaternion(_btQuat.w(), -_btQuat.x(), -_btQuat.y(), _btQuat.z());
        return Quaternion(_btQuat.w(), _btQuat.x(), _btQuat.y(), _btQuat.z());
    }

    inline Transform& bt_mx_cast(const btTransform& _btTrans, Transform& _mxTrans, const bool _switchHandness = true) noexcept {
        _mxTrans.setPosition(bt_mx_cast(_btTrans.getOrigin(), _switchHandness));
        _mxTrans.setRotation(bt_mx_cast(_btTrans.getRotation(), _switchHandness));
        return _mxTrans; // code symmetric
    }

    inline btVector3 mx_bt_cast(const Vector3f& _mxVec, const bool _switchHandness = true) noexcept {
        if(_switchHandness)
            return btVector3(_mxVec.x, _mxVec.y, -_mxVec.z);
        return btVector3(_mxVec.x, _mxVec.y, _mxVec.z);
    }

    inline btQuaternion mx_bt_cast(const Quaternion& _mxQuat, const bool _switchHandness = true) noexcept {
        if(_switchHandness)
            return btQuaternion(-_mxQuat.x, -_mxQuat.y, _mxQuat.z, _mxQuat.w);
        return btQuaternion(_mxQuat.x, _mxQuat.y, _mxQuat.z, _mxQuat.w);
    }

    inline btTransform mx_bt_cast(const Transform& _mxTrans, const bool _switchHandness = true) noexcept {
        return btTransform(mx_bt_cast(_mxTrans.getRotation(), _switchHandness),
                           mx_bt_cast(_mxTrans.getPosition(), _switchHandness));
    }

    //inline Transform& bt_mx_cast(const btTransform& _btTrans, Transform& _mxTrans) noexcept {
    //    _mxTrans.setPosition({
    //        -_btTrans.getOrigin().x(),
    //        _btTrans.getOrigin().y(),
    //        _btTrans.getOrigin().z()
    //    });
    //    _mxTrans.setRotation({
    //        -_btTrans.getRotation().w(),
    //        -_btTrans.getRotation().x(),
    //        _btTrans.getRotation().y(),
    //        _btTrans.getRotation().z()
    //    });
    //    return _mxTrans; // code symmetric
    //}

    //inline btTransform mx_bt_cast(const Transform& _mxTrans) noexcept {
    //    return btTransform({
    //                           -_mxTrans.getRotation().x,
    //                           _mxTrans.getRotation().y,
    //                           _mxTrans.getRotation().z,
    //                           -_mxTrans.getRotation().w
    //                       },
    //                       {
    //                           -_mxTrans.getPosition().x,
    //                           _mxTrans.getPosition().y,
    //                           _mxTrans.getPosition().z
    //                       });
    //}
}

#endif

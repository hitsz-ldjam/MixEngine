#pragma once

#ifndef MX_PHYSICS_UTILS_HPP_
#define MX_PHYSICS_UTILS_HPP_

#include "../Component/Transform/MxTransform.h"
//#include <bullet3/LinearMath/btTransform.h>
#include <bullet3/btBulletDynamicsCommon.h>

namespace Mix::Physics {
    inline void btTransToMixTrans(const btTransform& _btTrans, Mix::Transform& _mixTrans) noexcept {
        _mixTrans.setPosition({
            -_btTrans.getOrigin().x(),
            _btTrans.getOrigin().y(),
            _btTrans.getOrigin().z()
        });
        _mixTrans.setRotation({
            -_btTrans.getRotation().w(),
            -_btTrans.getRotation().x(),
            _btTrans.getRotation().y(),
            _btTrans.getRotation().z()
        });
    }

    inline btTransform mixTransToBtTrans(const Mix::Transform& _mixTrans) noexcept {
        return btTransform({
                               -_mixTrans.getRotation().x,
                               _mixTrans.getRotation().y,
                               _mixTrans.getRotation().z,
                               -_mixTrans.getRotation().w
                           },
                           {
                               -_mixTrans.getPosition().x,
                               _mixTrans.getPosition().y,
                               _mixTrans.getPosition().z
                           });
    }
}

#endif

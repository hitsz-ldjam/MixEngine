#pragma once

#ifndef MX_PHYSICS_UTILS_HPP_
#define MX_PHYSICS_UTILS_HPP_

#include "../Component/Transform/MxTransform.h"
//#include <bullet3/LinearMath/btTransform.h>
#include <bullet3/btBulletDynamicsCommon.h>

namespace Mix::Physics {
    // todo: debug. bullet uses right hand coordinate system!
    inline Transform btTransformToMixTransform(const btTransform& _btTrans) noexcept {
        Transform trans;
        trans.setPosition({
            _btTrans.getOrigin().x(),
            _btTrans.getOrigin().y(),
            _btTrans.getOrigin().z()
        });
        trans.setRotation({
            _btTrans.getRotation().w(),
            _btTrans.getRotation().x(),
            _btTrans.getRotation().y(),
            _btTrans.getRotation().z()
        });
        return trans;
    }

    inline btTransform mixTransformToBtTransform(const Transform& _mixTrans) noexcept {
        return btTransform({
                               _mixTrans.getRotation().x,
                               _mixTrans.getRotation().y,
                               _mixTrans.getRotation().z,
                               _mixTrans.getRotation().w
                           },
                           {
                               _mixTrans.getPosition().x,
                               _mixTrans.getPosition().y,
                               _mixTrans.getPosition().z
                           });
    }
}

#endif

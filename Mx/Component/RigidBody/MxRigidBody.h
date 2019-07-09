#pragma once

#ifndef MX_RIGID_BODY_H_
#define MX_RIGID_BODY_H_

#include "../Behaviour/MxBehaviour.h"
#include <bullet3/btBulletDynamicsCommon.h>

namespace Mix {
    // todo: handle collision events
    class RigidBody final : public Behaviour {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        /** @note Default ctor is for RTTI. DO NOT use this ctor. */
        RigidBody();
        RigidBody(const btScalar _mass,
                  const btTransform& _startTrans,
                  std::shared_ptr<btCollisionShape> _shape);
        RigidBody(const RigidBody&) = delete;
        ~RigidBody();

        void awake() override;

        auto& get() noexcept { return *mRigidBody; }
        const auto& get() const noexcept { return *mRigidBody; }

    private:
        btScalar mMass;
        btTransform mTransform;
        std::shared_ptr<btCollisionShape> mShape;
        btRigidBody* mRigidBody;
        btDiscreteDynamicsWorld* mWorld;
    };
}

#endif

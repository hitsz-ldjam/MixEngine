#include "MxRigidBody.h"
#include "../../Physics/MxPhysicsWorld.h"
#include "../../Physics/MxPhysics.hpp"
#include "../../../MixEngine.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(RigidBody, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(RigidBody)

    RigidBody::RigidBody() : mMass(.0),
                             mTransform(btTransform::getIdentity()),
                             mShape(nullptr),
                             mRigidBody(nullptr),
                             mWorld(nullptr) {}

    RigidBody::RigidBody(const btScalar _mass,
                         const btTransform& _startTrans,
                         std::shared_ptr<btCollisionShape> _shape) : mMass(_mass),
                                                                     mTransform(_startTrans),
                                                                     mShape(std::move(_shape)),
                                                                     mRigidBody(nullptr),
                                                                     mWorld(nullptr) {}

    RigidBody::~RigidBody() {
        if(mRigidBody && mWorld) {
            mWorld->removeRigidBody(mRigidBody);
            delete mRigidBody->getMotionState();
        }
        delete mRigidBody;
    }

    void RigidBody::awake() {
        if(!mShape)
            return;
        mWorld = MixEngine::Instance().getModule<Physics::World>()->getWorld();
        auto motionState = new Physics::MotionState(mTransform);
        btVector3 localInertia(0, 0, 0);
        if(mMass > FLT_EPSILON)
            mShape->calculateLocalInertia(mMass, localInertia);
        btRigidBody::btRigidBodyConstructionInfo info(mMass, motionState, mShape.get(), localInertia);
        mRigidBody = new btRigidBody(info);
        mRigidBody->setUserPointer(this);
        mWorld->addRigidBody(mRigidBody);
    }
}

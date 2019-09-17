#include "MxRigidBody.h"
#include "../../Physics/MxPhysicsWorld.h"
#include "../../Physics/MxRigidBodyUtils.hpp"
#include "../../Physics/MxPhysicsUtils.hpp"
#include "../../GameObject/MxGameObject.h"
#include "../../../MixEngine.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(RigidBody, Behaviour)

    struct RigidBody::LifeTimeManager {
        ~LifeTimeManager() {
            for(auto* shape : shapes)
                delete shape;
        }

        void addShape(btCollisionShape* _shape) {
            if(!_shape) return;
            if(_shape->isCompound()) {
                auto cs = static_cast<btCompoundShape*>(_shape);
                for(auto idx = cs->getNumChildShapes() - 1; idx >= 0; --idx)
                    addShape(cs->getChildShape(idx));
            }
            shapes.insert(_shape);
        }

        //btAlignedObjectArray<btCollisionShape*> shapes;
        std::set<btCollisionShape*> shapes;
    };

    RigidBody::LifeTimeManager RigidBody::ltm;

    RigidBody::RigidBody() : mMass(.0),
                             mShape(nullptr),
                             mRigidBody(nullptr),
                             mFilter(false, 0, 0),
                             mWorld(nullptr) {}

    RigidBody::RigidBody(const btScalar _mass,
                         const btTransform& _startTrans,
                         btCollisionShape* _shape) : mMass(_mass),
                                                     mShape(_shape),
                                                     mRigidBody(nullptr),
                                                     mFilter(false, 0, 0),
                                                     mWorld(nullptr) {
        ltm.addShape(mShape);
        mRigidBody = CreateBtRb(Physics::RigidBodyConstructionInfo(_mass, _startTrans, _shape));
        mRigidBody->setUserPointer(this);
    }

    RigidBody::RigidBody(const btScalar _mass, const Transform& _startTrans, btCollisionShape* _shape)
        : RigidBody(_mass, Physics::mixTransToBtTrans(_startTrans), _shape) {}

    RigidBody::RigidBody(const Physics::RigidBodyConstructionInfo& _info) : mMass(_info.mass),
                                                                            mShape(_info.shape),
                                                                            mRigidBody(nullptr),
                                                                            mFilter(false, 0, 0),
                                                                            mWorld(nullptr) {
        ltm.addShape(mShape);
        mRigidBody = CreateBtRb(_info);
        mRigidBody->setUserPointer(this);
    }

    RigidBody::RigidBody(const Physics::RigidBodyConstructionInfo& _info,
                         const int _group,
                         const int _mask) : mMass(_info.mass),
                                            mShape(_info.shape),
                                            mRigidBody(nullptr),
                                            mFilter(true, _group, _mask),
                                            mWorld(nullptr) {
        ltm.addShape(mShape);
        mRigidBody = CreateBtRb(_info);
        mRigidBody->setUserPointer(this);
    }

    RigidBody::~RigidBody() {
        mEnterSignal.disconnect_all_slots();
        mExitSignal.disconnect_all_slots();
        if(mRigidBody) {
            if(mWorld)
                mWorld->removeRigidBody(mRigidBody);
            delete mRigidBody->getMotionState();
        }
        delete mRigidBody;
    }

    void RigidBody::setKinematic(const bool _flag) const {
        if(_flag) {
            addCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
            mRigidBody->setActivationState(DISABLE_DEACTIVATION);
        }
        else {
            removeCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
            mRigidBody->forceActivationState(ACTIVE_TAG);
        }
        forceReload();
    }

    void RigidBody::setStatic(const bool _flag) const {
        btScalar mass;
        btVector3 inertia(0, 0, 0);
        if(_flag)
            mass = 0;
        else {
            mass = mMass;
            mShape->calculateLocalInertia(mass, inertia);
        }
        mRigidBody->setMassProps(mass, inertia);
        mRigidBody->updateInertiaTensor();
        forceReload();
    }

    void RigidBody::setTrigger(const bool _flag) const {
        if(_flag)
            addCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        else
            removeCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    btRigidBody* RigidBody::CreateBtRb(const Physics::RigidBodyConstructionInfo& _info) {
        if(!_info.shape)
            return nullptr;
        auto motionstate = new Physics::MotionState(_info.startTrans,
                                                    _info.interpolation,
                                                    _info.centerOfMassOffset);
        btVector3 inertia(0, 0, 0);
        if(_info.mass > FLT_EPSILON)
            _info.shape->calculateLocalInertia(_info.mass, inertia);
        btRigidBody::btRigidBodyConstructionInfo info(_info.mass,
                                                      motionstate,
                                                      _info.shape,
                                                      inertia);
        //info.m_linearSleepingThreshold = .005;
        //info.m_angularSleepingThreshold = .01;
        _info.furtherSetup(info);
        return new btRigidBody(info);
    }

    void RigidBody::awake() {
        mWorld = MixEngine::Instance().getModule<Physics::World>()->getWorld();
        const auto& [customFilter, group, mask] = mFilter;
        if(customFilter)
            mWorld->addRigidBody(mRigidBody, group, mask);
        else
            mWorld->addRigidBody(mRigidBody);
    }

    void RigidBody::update() {
        //                                 Should be Physics::MotionState
        Physics::btTransToMixTrans(static_cast<Physics::MotionState*>(mRigidBody->getMotionState())
                                   ->interpolatedTransform(), mGameObject->transform());
    }

    void RigidBody::fixedUpdate() {
        btTransform trans;
        mRigidBody->getMotionState()->getWorldTransform(trans);
        Physics::btTransToMixTrans(trans, mGameObject->transform());
    }
}

#include "MxRigidBody.h"
#include "../../Physics/MxPhysicsWorld.h"
#include "../../Physics/MxPhysicsUtils.hpp"
#include "../../Physics/MxRigidBodyUtils.hpp"
#include "../../GameObject/MxGameObject.h"

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

    RigidBody::RigidBody(const btScalar _mass, const Transform& _startTrans, btCollisionShape* _shape)
        : RigidBody(Physics::RigidBodyConstructionInfo(_mass, Physics::mx_bt_cast(_startTrans), _shape)) {}

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
        _forceReload();
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
        _forceReload();
    }

    void RigidBody::setTrigger(const bool _flag) const {
        if(_flag)
            addCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
        else
            removeCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    }

    Vector3<float> RigidBody::getPosition() const {
        return Physics::bt_mx_cast(mRigidBody->getWorldTransform().getOrigin());
    }

    void RigidBody::setPosition(const Vector3<float>& _pos) const {
        mRigidBody->getWorldTransform().setOrigin(Physics::mx_bt_cast(_pos));
    }

    void RigidBody::translate(const Vector3<float>& _delta) const {
        mRigidBody->translate(Physics::mx_bt_cast(_delta));
    }

    Quaternion RigidBody::getRotation() const {
        return Physics::bt_mx_cast(mRigidBody->getWorldTransform().getRotation());
    }

    void RigidBody::setRotation(const Quaternion& _rot) const {
        mRigidBody->getWorldTransform().setRotation(Physics::mx_bt_cast(_rot));
    }

    void RigidBody::rotate(const Quaternion& _delta) const {
        //setRotation(getRotation() * _delta);
        setRotation(_delta * getRotation());
    }

    Vector3<float> RigidBody::getLinearVelocity() const {
        return Physics::bt_mx_cast(mRigidBody->getLinearVelocity());
    }

    void RigidBody::setLinearVelocity(const Vector3<float>& _vel) const {
        mRigidBody->setLinearVelocity(Physics::mx_bt_cast(_vel));
    }

    Vector3<float> RigidBody::getAngularVelocity() const {
        return -Physics::bt_mx_cast(mRigidBody->getAngularVelocity());
    }

    void RigidBody::setAngularVelocity(const Vector3<float>& _vel) const {
        mRigidBody->setAngularVelocity(-Physics::mx_bt_cast(_vel));
    }

    Vector3<float> RigidBody::getForce() const {
        return Physics::bt_mx_cast(mRigidBody->getTotalForce());
    }

    void RigidBody::addForce(const Vector3<float>& _force) const {
        mRigidBody->applyCentralForce(Physics::mx_bt_cast(_force));
    }

    void RigidBody::addForceAtPosition(const Vector3<float>& _force, const Vector3<float>& _pos) const {
        mRigidBody->applyForce(Physics::mx_bt_cast(_force), Physics::mx_bt_cast(_pos));
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
        info.m_linearSleepingThreshold = .02;  // default to .8
        info.m_angularSleepingThreshold = .02; // default to 1
        _info.furtherSetup(info);
        return new btRigidBody(info);
    }

    void RigidBody::awake() {
        mWorld = Physics::World::Get()->getWorld();
        const auto& [customFilter, group, mask] = mFilter;
        if(customFilter)
            mWorld->addRigidBody(mRigidBody, group, mask);
        else
            mWorld->addRigidBody(mRigidBody);
    }

    void RigidBody::update() {
        auto ms = static_cast<Physics::MotionState*>(mRigidBody->getMotionState());
        if(ms->interpolation() == Physics::RigidbodyInterpolation::NONE)
            return;
        // todo: sync by Transform
        Physics::bt_mx_cast(ms->interpolatedTransform(), mGameObject->transform());
    }

    void RigidBody::fixedUpdate() {
        //                         Should be Physics::MotionState
        Physics::bt_mx_cast(static_cast<Physics::MotionState*>(mRigidBody->getMotionState())
                            ->getWorldTransform(), mGameObject->transform());
    }
}

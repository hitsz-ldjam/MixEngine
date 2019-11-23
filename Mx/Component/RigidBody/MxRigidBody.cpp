#include "MxRigidBody.h"
#include "../../Physics/MxPhysicsWorld.h"
#include "../../Physics/MxPhysicsUtils.hpp"
#include "../../Physics/MxRigidBodyUtils.hpp"
#include "../../GameObject/MxGameObject.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(RigidBody, Behaviour)

    RigidBody::RigidBody() : mMass(.0),
                             mShape(nullptr),
                             mRigidBody(nullptr),
                             mFilter(false, 0, 0),
                             mWorld(nullptr) {}

    RigidBody::RigidBody(const btScalar _mass, const Transform& _startTrans, const std::shared_ptr<btCollisionShape>& _shape)
        : RigidBody(Physics::RigidBodyConstructionInfo(_mass, Physics::mx_bt_cast(_startTrans), _shape.get())) {
        mShape = _shape;
    }

    RigidBody::RigidBody(const Physics::RigidBodyConstructionInfo& _info) : mMass(_info.mass),
                                                                            mShape(nullptr),
                                                                            mRigidBody(nullptr),
                                                                            mFilter(false, 0, 0),
                                                                            mWorld(nullptr) {
        mRigidBody = CreateBtRb(_info);
        mRigidBody->setUserPointer(&mThisHandle);
    }

    RigidBody::RigidBody(const Physics::RigidBodyConstructionInfo& _info,
                         const int _group,
                         const int _mask) : mMass(_info.mass),
                                            mShape(nullptr),
                                            mRigidBody(nullptr),
                                            mFilter(true, _group, _mask),
                                            mWorld(nullptr) {
        mRigidBody = CreateBtRb(_info);
        mRigidBody->setUserPointer(&mThisHandle);
    }

    RigidBody::~RigidBody() {
        mEnterSignal.disconnect_all_slots();
        mExitSignal.disconnect_all_slots();
        if(mRigidBody) {
            if(mWorld && mRigidBody->isInWorld())
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
            mRigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
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

    Vector3<bool> RigidBody::getLinearRestrictions() const {
        const auto& factor = mRigidBody->getLinearFactor();
        return {btFuzzyZero(factor.x()), btFuzzyZero(factor.y()), btFuzzyZero(factor.z()),};
    }

    void RigidBody::setLinearRestrictions(const Vector3<bool>& _freeze) const {
        mRigidBody->setLinearFactor({!_freeze.x, !_freeze.y, !_freeze.z});
    }

    Vector3<bool> RigidBody::getAngularRestrictions() const {
        const auto& factor = mRigidBody->getAngularFactor();
        return {btFuzzyZero(factor.x()), btFuzzyZero(factor.y()), btFuzzyZero(factor.z()),};
    }

    void RigidBody::setAngularRestrictions(const Vector3<bool>& _freeze) const {
        mRigidBody->setAngularFactor({!_freeze.x, !_freeze.y, !_freeze.z});
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

    void RigidBody::addCollisionFlags(const int _flag) const {
        mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | _flag);
    }

    void RigidBody::removeCollisionFlags(const int _flag) const {
        mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() & (~_flag));
    }

    void RigidBody::addRbToWorld() const {
        if(mRigidBody->isInWorld())
            return;
        const auto& [customFilter, group, mask] = mFilter;
        if(customFilter)
            mWorld->addRigidBody(mRigidBody, group, mask);
        else
            mWorld->addRigidBody(mRigidBody);
    }

    void RigidBody::forceReload() const {
        if(!mRigidBody->isInWorld())
            return;
        mWorld->removeRigidBody(mRigidBody);
        addRbToWorld();
    }

    void RigidBody::awake() {
        mWorld = Physics::World::Get()->getWorld();
        addRbToWorld();
    }

    void RigidBody::update() {
        auto ms = static_cast<Physics::MotionState*>(mRigidBody->getMotionState());
        if(ms->interpolation() == Physics::RigidbodyInterpolation::None)
            return;
        Physics::bt_mx_cast(ms->interpolatedTransform(), mGameObject->transform());
    }

    void RigidBody::fixedUpdate() {
        //                         should be Physics::MotionState
        Physics::bt_mx_cast(static_cast<Physics::MotionState*>(mRigidBody->getMotionState())->getWorldTransform(),
                            mGameObject->transform());
    }

    void RigidBody::onEnable() {
        addRbToWorld();
    }

    void RigidBody::onDisable() {
        if(mRigidBody->isInWorld())
            mWorld->removeRigidBody(mRigidBody);
    }
}

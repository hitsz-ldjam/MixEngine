#pragma once

#ifndef MX_RIGID_BODY_H_
#define MX_RIGID_BODY_H_

#include "../Behaviour/MxBehaviour.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include <boost/signals2.hpp>
#include <tuple>

namespace Mix {
    class Transform;
    class Quaternion;
    template<typename _Ty>
    class Vector3;

    namespace Physics {
        class World;
        struct RigidBodyConstructionInfo;
    }

    /** Bug: RigidBody on a child GameObject whose parent has one might not be altered correctly. */
    class RigidBody final : public Behaviour {
    MX_DECLARE_RTTI;

        friend Physics::World;

    public:
        using Signal = boost::signals2::signal<void(RigidBody*)>;
        using Slot = Signal::slot_type;

        /** @note Default ctor is for RTTI. DO NOT use this ctor. */
        RigidBody();
        /** @note Refer to Physics::RigidBodyConstructionInfo for param info. */
        RigidBody(const btScalar _mass, const btTransform& _startTrans, btCollisionShape* _shape);
        RigidBody(const btScalar _mass, const Transform& _startTrans, btCollisionShape* _shape);
        RigidBody(const Physics::RigidBodyConstructionInfo& _info);
        /**
         *  @param _group Filter group.
         *  @param _mask Filter mask. Indicates which groups this object should collide with.
         *  @note Filters should not conflict with built in groups. See btBroadphaseProxy::CollisionFilterGroups
         */
        RigidBody(const Physics::RigidBodyConstructionInfo& _info, const int _group, const int _mask);
        RigidBody(const RigidBody&) = delete;
        ~RigidBody();

        auto& get() noexcept { return *mRigidBody; }
        const auto& get() const noexcept { return *mRigidBody; }

        // ----- dirty codes -----
        // todo: move to an utility class

        /**
         *  @brief Register call back functions of onTriggerEnter()
         *  @param _slot The call back function.
         *  @note Lambdas are preferred. To register a member function outside class scope,
         *  see the example below.\n
         *  @code
         *  //ClassName* ptrToIns;
         *  registerSlot(boost::bind(&ClassName::slotFunc, ptrToIns, _1));
         *  @endcode
         */
        auto registerEnterSlot(const Slot& _slot) { return mEnterSignal.connect(_slot); }
        auto registerExitSlot(const Slot& _slot) { return mExitSignal.connect(_slot); }
        // ----- dirty codes -----

        void addCollisionFlags(const int _flag) const {
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() | _flag);
        }

        void removeCollisionFlags(const int _flag) const {
            mRigidBody->setCollisionFlags(mRigidBody->getCollisionFlags() & (~_flag));
        }

        bool isKinematic() const { return mRigidBody->isKinematicObject(); }
        void setKinematic(const bool _flag) const;

        bool isStatic() const { return mRigidBody->isStaticObject(); }
        /** @note Static objects should NOT be moved. */
        void setStatic(const bool _flag) const;

        bool isTrigger() const { return !mRigidBody->hasContactResponse(); }
        void setTrigger(const bool _flag) const;

        void disableDeactivation() const { mRigidBody->setActivationState(DISABLE_DEACTIVATION); }
        void enableDeactivation() const { mRigidBody->forceActivationState(ACTIVE_TAG); }

        Vector3<float> getPosition() const;
        void setPosition(const Vector3<float>& _pos) const;
        void translate(const Vector3<float>& _delta) const;

        Quaternion getRotation() const;
        void setRotation(const Quaternion& _rot) const;
        void rotate(const Quaternion& _delta) const;

        Vector3<float> getLinearVelocity() const;
        void setLinearVelocity(const Vector3<float>& _vel) const;
        Vector3<float> getAngularVelocity() const;
        void setAngularVelocity(const Vector3<float>& _vel) const;

        // todo: other interfaces

    private:
        struct LifeTimeManager;
        static LifeTimeManager ltm;

        btScalar mMass;
        btCollisionShape* mShape;
        btRigidBody* mRigidBody;
        std::tuple<bool, int, int> mFilter;
        btDiscreteDynamicsWorld* mWorld;

        Signal mEnterSignal, mExitSignal;
        void onTriggerEnter(RigidBody* _other) { mEnterSignal(_other); }
        void onTriggerExit(RigidBody* _other) { mExitSignal(_other); }

        /** @brief A utility function to create btRigidBody from RigidBodyConstructionInfo */
        static btRigidBody* CreateBtRb(const Physics::RigidBodyConstructionInfo& _info);

        void _forceReload() const {
            if(!mRigidBody->isInWorld())
                return;
            mWorld->removeRigidBody(mRigidBody);
            mWorld->addRigidBody(mRigidBody);
        }

        void awake() override;
        void update() override;
        void fixedUpdate() override;
    };
}

#endif

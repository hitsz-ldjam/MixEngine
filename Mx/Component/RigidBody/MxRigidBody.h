#pragma once

#ifndef MX_RIGID_BODY_H_
#define MX_RIGID_BODY_H_

#include "../Behaviour/MxBehaviour.h"
#include "../../Definitions/MxDefinitions.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include <boost/signals2.hpp>
#include <tuple>

namespace Mix {
    template<typename _Ty>
    class Vector3;
    class Quaternion;
    class Transform;

    namespace Physics {
        class World;
        struct RigidBodyConstructionInfo;
    }

    /** Bug: RigidBody on a child GameObject might not behave correctly. */
    class RigidBody final : public Behaviour {
    MX_DECLARE_RTTI;

        friend Physics::World;

    public:
        using Signal = boost::signals2::signal<void(HRigidBody)>;
        using Slot = Signal::slot_type;

        /** @note Default ctor is for RTTI. DO NOT use this ctor. */
        RigidBody();
        /**
         *  @param _mass Mass. 0 creates a static rigid body i.e. btCollisionObject::CF_STATIC_OBJECT
         *  @param _startTrans Start transform.
         *  @param _shape Collision shape / Collider. DO NOT use std::make_shared on btCollisionShape in case of which align problems may occur.
         */
        RigidBody(const btScalar _mass, const Transform& _startTrans, const std::shared_ptr<btCollisionShape>& _shape);
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
        // todo: use Event system

        /**
         *  @brief Register call back functions of onCollisionEnter()
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

        bool isKinematic() const { return mRigidBody->isKinematicObject(); }
        void setKinematic(const bool _flag) const;

        bool isStatic() const { return mRigidBody->isStaticObject(); }
        /** @note Static objects should NOT be moved. */
        void setStatic(const bool _flag) const;

        bool isTrigger() const { return !mRigidBody->hasContactResponse(); }
        void setTrigger(const bool _flag) const;

        /** @brief Disable deactivation of the rigid body thus it can be controlled by user input. */
        void disableDeactivation() const { mRigidBody->setActivationState(DISABLE_DEACTIVATION); }
        void enableDeactivation() const { mRigidBody->forceActivationState(ACTIVE_TAG); }

        Vector3<float> getPosition() const;
        void setPosition(const Vector3<float>& _pos) const;
        /** @brief Translate relative to world. */
        void translate(const Vector3<float>& _delta) const;

        Quaternion getRotation() const;
        void setRotation(const Quaternion& _rot) const;
        /** @brief Rotate relative to world. */
        void rotate(const Quaternion& _delta) const;

        Vector3<float> getLinearVelocity() const;
        void setLinearVelocity(const Vector3<float>& _vel) const;
        Vector3<float> getAngularVelocity() const;
        void setAngularVelocity(const Vector3<float>& _vel) const;

        Vector3<float> getForce() const;

        /** 
         *  @param _force Force in world coordinates.
         *  @note The force is only applied in one single fixed frame.
         */
        void addForce(const Vector3<float>& _force) const;

        /**
         *  @param _force Force in world coordinates.
         *  @param _pos Position in world coordinates.
         *  @note The force is only applied in one single fixed frame.
         */
        void addForceAtPosition(const Vector3<float>& _force, const Vector3<float>& _pos) const;

        /** @return @code true @endcode indicates the motion along the corresponding axis is frozen. */
        Vector3<bool> getLinearRestrictions() const;
        void setLinearRestrictions(const Vector3<bool>& _freeze) const;
        Vector3<bool> getAngularRestrictions() const;
        void setAngularRestrictions(const Vector3<bool>& _freeze) const;

        // todo: other interfaces

    private:
        btScalar mMass;
        std::shared_ptr<btCollisionShape> mShape;
        btRigidBody* mRigidBody;
        std::tuple<bool, int, int> mFilter;
        btDiscreteDynamicsWorld* mWorld;

        Signal mEnterSignal, mExitSignal;
        void onCollisionEnter(HRigidBody _other) { mEnterSignal(std::move(_other)); }
        void onCollisionExit(HRigidBody _other) { mExitSignal(std::move(_other)); }

        /** @brief A utility function that creates btRigidBody from RigidBodyConstructionInfo */
        static btRigidBody* CreateBtRb(const Physics::RigidBodyConstructionInfo& _info);


        void addCollisionFlags(const int _flag) const;
        void removeCollisionFlags(const int _flag) const;
        void addRbToWorld() const;
        void forceReload() const;

        void awake() override;
        void update() override;
        void fixedUpdate() override;
        void onEnabled() override;
        void onDisabled() override;
    };
}

#endif

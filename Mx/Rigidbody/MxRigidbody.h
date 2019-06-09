#pragma once
#ifndef MX_RIGIDBODY_H_
#define MX_RIGIDBODY_H_

#include <btBulletDynamicsCommon.h>

/// 冲突配置包含内存的默认设置，冲突设置。
btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

/// 使用默认的冲突调度程序。对于并行处理，
btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

/// btDbvtBroadphase是一种很好的通用的两步法碰撞检测。
btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

/// 默认约束求解器。
btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

namespace Mix {
	class Rigidbody
	{
	public:
		void getAabb(btVector3& aabbMin, btVector3& aabbMax);
		btScalar getAngularDamping();
		btScalar getAngularFactor();
		btScalar getAngularSleepingThreshold();
		btScalar getLinearSleepingThreshold();
		btCollisionShape* getCollisionShape();
		btVector3& getLinearFactor();
		btScalar getInvMass();
		const btVector3& getCenterOfMassPosition();
		const btTransform& getCenterOfMassTransform();
		const btVector3& getLinearVelocity();
		const btVector3& getAngularVelocity();
		btVector3 getVelocityInLocalPoint(const btVector3& rel_pos);
		const btBroadphaseProxy* getBroadphaseProxy();
		btTypedConstraint* getConstraintRef(int index);
		btMotionState* getMotionState();

		static const btRigidBody* upcast(const btCollisionObject* colObj);
		static btRigidBody* upcast(btCollisionObject* colObj);
		void predictIntegratedTransform(btScalar step, btTransform& predictedTransform);

		void saveKinematicState(btScalar step);

		void applyGravity();

		void setGravity(const btVector3& acceleration);
		void setDamping(btScalar lin_damping, btScalar ang_damping);

		void applyDamping(btScalar timeStep);
		void setMassProps(btScalar mass, const btVector3& inertia);
		void setLinearFactor(const btVector3& linearFactor);

		void integrateVelocities(btScalar step);

		void setCenterOfMassTransform(const btTransform& xform);

		void applyCentralForce(const btVector3& force);
		void setInvInertiaDiagLocal(const btVector3& diagInvInertia);
		void setSleepingThresholds(btScalar linear, btScalar angular);
		void applyTorque(const btVector3& torque);
		void applyForce(const btVector3& force, const btVector3& rel_pos);
		void applyCentralImpulse(const btVector3& impulse);
		void applyTorqueImpulse(const btVector3& torque);
		void applyImpulse(const btVector3& impulse, const btVector3& rel_pos);
		void clearForces();
		void updateInertiaTensor();

		inline void setLinearVelocity(const btVector3& lin_vel);
		inline void setAngularVelocity(const btVector3& ang_vel);

		void translate(const btVector3& v);

		btScalar computeImpulseDenominator(const btVector3& pos, const btVector3& normal);
		btScalar computeAngularImpulseDenominator(const btVector3& axis);
		void updateDeactivation(btScalar timeStep);
		bool wantsSleeping();
		void setNewBroadphaseProxy(btBroadphaseProxy* broadphaseProxy);
		void setMotionState(btMotionState* motionState);
		void setAngularFactor(const btVector3& angFac);
		void setAngularFactor(btScalar angFac);
		bool isInWorld();

		void addConstraintRef(btTypedConstraint* c);
		void removeConstraintRef(btTypedConstraint* c);

		void setFlags(int flags);

		///perform implicit force computation in world space
		btVector3 computeGyroscopicImpulseImplicit_World(btScalar dt) const;

		///perform implicit force computation in body space (inertial frame)
		btVector3 computeGyroscopicImpulseImplicit_Body(btScalar step) const;

		///explicit version is best avoided, it gains energy
		btVector3 computeGyroscopicForceExplicit(btScalar maxGyroscopicForce) const;
		btVector3 getLocalInertia() const;
		virtual int calculateSerializeBufferSize() const;

		///fills the dataBuffer and returns the struct name (and 0 on failure)
		virtual const char* serialize(void* dataBuffer, class btSerializer* serializer) const;

		virtual void serializeSingleObject(class btSerializer* serializer) const;

	private:
		btRigidBody* rb;

	};



}

#endif
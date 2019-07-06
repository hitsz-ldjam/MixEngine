#include "MxRigidbody.h"

namespace Mix {
	void Rigidbody::addConstraintRef(btTypedConstraint* c) {
		rb->addConstraintRef(c);
	}
	void Rigidbody::getAabb(btVector3& aabbMin, btVector3& aabbMax) {
		rb->getAabb(aabbMin, aabbMax);
	}
	btScalar Rigidbody::getAngularDamping() {
		rb->getAngularDamping();
	}
	btScalar Rigidbody::getAngularFactor() {
		rb->getAngularFactor();
	}
	btScalar Rigidbody::getAngularSleepingThreshold() {
		rb->getAngularSleepingThreshold();
	}
	btScalar Rigidbody::getLinearSleepingThreshold() {
		rb->getLinearSleepingThreshold();
	}
	btCollisionShape* Rigidbody::getCollisionShape() {
		rb->getCollisionShape();
	}
	btVector3& Rigidbody::getLinearFactor() {
		rb->getLinearFactor();
	}
	btScalar Rigidbody::getInvMass() {
		rb->getInvMass();
	}
	const btVector3& Rigidbody::getCenterOfMassPosition() {
		rb->getCenterOfMassPosition();
	}
	const btTransform& Rigidbody::getCenterOfMassTransform() {
		rb->getCenterOfMassTransform();
	}
	const btVector3& Rigidbody::getLinearVelocity() {
		rb->getLinearVelocity();
	}
	const btVector3& Rigidbody::getAngularVelocity() {
		rb->getAngularVelocity();
	}
	btVector3 Rigidbody::getVelocityInLocalPoint(const btVector3& rel_pos) {
		rb->getVelocityInLocalPoint(rel_pos);
	}
	const btBroadphaseProxy* Rigidbody::getBroadphaseProxy() {
		rb->getBroadphaseProxy();
	}
	btTypedConstraint* Rigidbody::getConstraintRef(int index) {
		rb->getConstraintRef(index);
	}
	btMotionState* Rigidbody::getMotionState() {
		rb->getMotionState();
	}

	const btRigidBody* Rigidbody::upcast(const btCollisionObject* colObj) {
		rb->upcast(colObj);
	}
	btRigidBody* Rigidbody::upcast(btCollisionObject* colObj) {
		rb->upcast(colObj);
	}
	void Rigidbody::predictIntegratedTransform(btScalar step, btTransform& predictedTransform) {
		rb->predictIntegratedTransform(step, predictedTransform);
	}

	void Rigidbody::saveKinematicState(btScalar step) {
		rb->saveKinematicState(step);
	}

	void Rigidbody::applyGravity() {
		rb->applyGravity();
	}

	void Rigidbody::setGravity(const btVector3& acceleration) {
		rb->setGravity(acceleration);
	}
	void Rigidbody::setDamping(btScalar lin_damping, btScalar ang_damping) {
		rb->setDamping(lin_damping, ang_damping);
	}

	void Rigidbody::applyDamping(btScalar timeStep) {
		rb->applyDamping(timeStep);
	}
	void Rigidbody::setMassProps(btScalar mass, const btVector3& inertia) {
		rb->setMassProps(mass, inertia);
	}
	void Rigidbody::setLinearFactor(const btVector3& linearFactor) {
		rb->setLinearFactor(linearFactor);
	}

	void Rigidbody::integrateVelocities(btScalar step) {
		rb->integrateVelocities(step);
	}

	void Rigidbody::setCenterOfMassTransform(const btTransform& xform) {
		rb->setCenterOfMassTransform(xform);
	}

	void Rigidbody::applyCentralForce(const btVector3& force) {
		rb->applyCentralForce(force);
	}
	void Rigidbody::setInvInertiaDiagLocal(const btVector3& diagInvInertia) {
		rb->setInvInertiaDiagLocal(diagInvInertia);
	}
	void Rigidbody::setSleepingThresholds(btScalar linear, btScalar angular) {
		rb->setSleepingThresholds(linear, angular);
	}
	void Rigidbody::applyTorque(const btVector3& torque) {
		rb->applyTorque(torque);
	}
	void Rigidbody::applyForce(const btVector3& force, const btVector3& rel_pos) {
		rb->applyForce(force, rel_pos);
	}
	void Rigidbody::applyCentralImpulse(const btVector3& impulse) {
		rb->applyCentralImpulse(impulse);
	}
	void Rigidbody::applyTorqueImpulse(const btVector3& torque) {
		rb->applyTorqueImpulse(torque);
	}
	void Rigidbody::applyImpulse(const btVector3& impulse, const btVector3& rel_pos) {
		rb->applyImpulse(impulse, rel_pos);
	}
	void Rigidbody::clearForces() {
		rb->clearForces();
	}
	void Rigidbody::updateInertiaTensor() {
		rb->updateInertiaTensor();
	}

	inline void Rigidbody::setLinearVelocity(const btVector3& lin_vel) {
		rb->setLinearVelocity(lin_vel);
	}
	inline void Rigidbody::setAngularVelocity(const btVector3& ang_vel) {
		rb->setAngularVelocity(ang_vel);
	}

	void Rigidbody::translate(const btVector3& v) {
		rb->translate(v);
	}

	btScalar Rigidbody::computeImpulseDenominator(const btVector3& pos, const btVector3& normal) {
		rb->computeImpulseDenominator(pos, normal);
	}
	btScalar Rigidbody::computeAngularImpulseDenominator(const btVector3& axis) {
		rb->computeAngularImpulseDenominator(axis);
	}
	void Rigidbody::updateDeactivation(btScalar timeStep) {
		rb->updateDeactivation(timeStep);
	}
	bool Rigidbody::wantsSleeping() {
		rb->wantsSleeping();
	}
	void Rigidbody::setNewBroadphaseProxy(btBroadphaseProxy* broadphaseProxy) {
		rb->setNewBroadphaseProxy(broadphaseProxy);
	}
	void Rigidbody::setMotionState(btMotionState* motionState) {
		rb->setMotionState(motionState);
	}
	void Rigidbody::setAngularFactor(const btVector3& angFac) {
		rb->setAngularFactor(angFac);
	}
	void Rigidbody::setAngularFactor(btScalar angFac) {
		rb->setAngularFactor(angFac);
	}
	bool Rigidbody::isInWorld() {
		rb->isInWorld();
	}

	
	void Rigidbody::removeConstraintRef(btTypedConstraint* c) {
		rb->removeConstraintRef(c);
	}

	void Rigidbody::setFlags(int flags) {
		rb->setFlags(flags);
	}

	///perform implicit force computation in world space
	btVector3 Rigidbody::computeGyroscopicImpulseImplicit_World(btScalar dt) {
		rb->computeGyroscopicImpulseImplicit_World(dt);
	}

	///perform implicit force computation in body space (inertial frame)
	btVector3 Rigidbody::computeGyroscopicImpulseImplicit_Body(btScalar step) {
		rb->computeGyroscopicImpulseImplicit_Body(step);
	}

	///explicit version is best avoided, it gains energy
	btVector3 Rigidbody::computeGyroscopicForceExplicit(btScalar maxGyroscopicForce) {
		rb->computeGyroscopicForceExplicit(maxGyroscopicForce);
	}
	btVector3 Rigidbody::getLocalInertia() {
		rb->getLocalInertia();
	}
	int Rigidbody::calculateSerializeBufferSize() {
		rb->calculateSerializeBufferSize();
	 }

	///fills the dataBuffer and returns the struct name (and 0 on failure)
	const char* Rigidbody::serialize(void* dataBuffer, class btSerializer* serializer) {
		rb->serialize(dataBuffer, serializer);
	 }

	void Rigidbody::serializeSingleObject(class btSerializer* serializer) {
		rb->serializeSingleObject(serializer);
	 }
}
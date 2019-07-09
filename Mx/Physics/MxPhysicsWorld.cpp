#include "MxPhysicsWorld.h"
#include "MxPhysics.hpp"
#include "../Component/RigidBody/MxRigidBody.h"
#include "../GameObject/MxGameObject.h"

namespace Mix::Physics {
    World::World() : mConfig(nullptr),
                     mDispatcher(nullptr),
                     mBroadphase(nullptr),
                     mSolver(nullptr),
                     mWorld(nullptr) {}

    World::~World() {
        delete mWorld;
        delete mSolver;
        delete mBroadphase;
        delete mDispatcher;
        delete mConfig;
    }

    void World::awake() {
        try {
            mConfig = new btDefaultCollisionConfiguration;
            mDispatcher = new btCollisionDispatcher(mConfig);
            mBroadphase = new btDbvtBroadphase;
            mSolver = new btSequentialImpulseConstraintSolver;
            mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mConfig);
        }
        catch(const std::exception&) {
            throw ThirdPartyLibInitError("Bullet");
        }

        // "Scientific Correctness"
        mWorld->setGravity(btVector3(0, -9.8f, 0));
    }

    void World::step(const btScalar _fixedDeltaTime) {
        mWorld->stepSimulation(_fixedDeltaTime, 0, btScalar(0) /* if 2nd param is 0, 3rd param is not used */);
        for(auto i = mWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
            auto colObj = mWorld->getCollisionObjectArray()[i];
            auto rb = btRigidBody::upcast(colObj);
            btTransform trans;
            if(rb && rb->getMotionState())
                rb->getMotionState()->getWorldTransform(trans);
            else
                trans = colObj->getWorldTransform();
            static_cast<RigidBody*>(colObj->getUserPointer())->getGameObject()->transform()
                = btTransformToMixTransform(trans);
        }

        // todo: process collision events
    }

    void World::sync(const btScalar _fixedDeltaTime, const btScalar _smoothing) {
        for(auto i = mWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
            auto colObj = mWorld->getCollisionObjectArray()[i];
            auto rb = btRigidBody::upcast(colObj);
            if(rb) {
                // PLEASE use Mix::Physics::MotionState PLEASE
                auto motionState = static_cast<MotionState*>(rb->getMotionState());
                motionState->calculateInterpolatedTransform(_fixedDeltaTime, _smoothing);
                static_cast<RigidBody*>(rb->getUserPointer())->getGameObject()->transform()
                    = btTransformToMixTransform(motionState->interpolatedTransform());
            }
        }
    }
}

#include "MxPhysicsWorld.h"
#include "MxRigidBodyUtils.hpp"
#include "../Component/RigidBody/MxRigidBody.h"
#include "../../MixEngine.h"

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

    void World::load() {
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
        processCollisions();
    }

    void World::sync(const btScalar _fixedDeltaTime, const btScalar _smoothing) {
        for(auto i = mWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
            auto colObj = mWorld->getCollisionObjectArray()[i];
            auto rb = btRigidBody::upcast(colObj);
            if(rb) {
                // PLEASE use Physics::MotionState PLEASE
                auto motionState = static_cast<MotionState*>(rb->getMotionState());
                motionState->calculateInterpolatedTransform(_fixedDeltaTime, _smoothing);
            }
        }
    }

    void World::processCollisions() {
        CollisionSet currSet;
        for(auto manifoldIdx = mDispatcher->getNumManifolds() - 1; manifoldIdx >= 0; --manifoldIdx) {
            auto manifold = mDispatcher->getManifoldByIndexInternal(manifoldIdx);
            if(manifold->getNumContacts() == 0)
                continue;

            // todo: play with manifolds
            //for(auto contactIdx = manifold->getNumContacts() - 1; contactIdx >= 0; --contactIdx)
            //    manifold->getContactPoint(contactIdx);

            auto colObj0 = manifold->getBody0();
            auto colObj1 = manifold->getBody1();

            // make colObj0 < colObj1
            // swap manually in case std::swap be specialized
            if(colObj0 > colObj1) {
                auto temp = colObj0;
                colObj0 = colObj1;
                colObj1 = temp;
            }

            auto colPair = std::make_pair(colObj0, colObj1);
            currSet.insert(colPair);
            if(prevSet.find(colPair) == prevSet.end()) {
                auto rb0 = static_cast<RigidBody*>(colObj0->getUserPointer());
                auto rb1 = static_cast<RigidBody*>(colObj1->getUserPointer());
                rb0->onTriggerEnter(rb1);
                rb1->onTriggerEnter(rb0);
            }
        }

        CollisionSet diffSet;
        std::set_difference(prevSet.begin(),
                            prevSet.end(),
                            currSet.begin(),
                            currSet.end(),
                            std::inserter(diffSet, diffSet.begin()));
        for(const auto& colPair : diffSet) {
            auto rb0 = static_cast<RigidBody*>(colPair.first->getUserPointer());
            auto rb1 = static_cast<RigidBody*>(colPair.second->getUserPointer());
            rb0->onTriggerExit(rb1);
            rb1->onTriggerExit(rb0);
        }

        prevSet = std::move(currSet);
    }
}

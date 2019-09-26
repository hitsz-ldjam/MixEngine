#include "MxPhysicsWorld.h"
#include "MxPhysicsUtils.hpp"
#include "MxRigidBodyUtils.hpp"
#include "../Component/RigidBody/MxRigidBody.h"
#include "../Math/MxVector3.h"
#include "../../MixEngine.h"

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
#include "MxPhysicsDebugDraw.h"
#endif

namespace Mix::Physics {
    World::World() :
#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
        mDebugDraw(nullptr),
#endif
        mConfig(nullptr),
        mDispatcher(nullptr),
        mBroadphase(nullptr),
        mSolver(nullptr),
        mWorld(nullptr),
        mIsPaused(false) {}

    World::~World() {
#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
        mWorld->setDebugDrawer(nullptr);
        delete mDebugDraw;
#endif
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

            // "Scientific Correctness"
            mWorld->setGravity(btVector3(0, -9.8f, 0));

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
            mDebugDraw = new DebugDraw;
            mDebugDraw->awake();
            mDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
            mWorld->setDebugDrawer(mDebugDraw);
#endif

        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            throw ThirdPartyLibInitError("Bullet");
        }
    }

    void World::step(const btScalar _fixedDeltaTime) {
        if(!mIsPaused) {
            mWorld->stepSimulation(_fixedDeltaTime, 0, btScalar(0) /* if 2nd param is 0, 3rd param is not used */);
            processCollisions();
        }
    }

    void World::sync(const btScalar _fixedDeltaTime, const btScalar _smoothing) const {
        if(!mIsPaused)
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

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
    void World::pushDrawData() const {
        if(mWorld->getDebugDrawer()) {
            mDebugDraw->drawLine({0, 0, 0}, {1, 0, 0}, {1, 0, 0});
            mDebugDraw->drawLine({0, 0, 0}, {0, 1, 0}, {0, 1, 0});
            mDebugDraw->drawLine({0, 0, 0}, {0, 0, 1}, {0, 0, 1});
            mWorld->debugDrawWorld();
        }
    }

    void World::render() const { mDebugDraw->render(); }
#endif

    RaycastHit World::raycast(const Vector3<float>& _origin,
                              const Vector3<float>& _direction,
                              const float _maxDistance) const {
        btCollisionWorld::ClosestRayResultCallback callback({}, {});
        return raycast(_origin, _direction, _maxDistance, callback.m_collisionFilterGroup, callback.m_collisionFilterMask);
    }

    RaycastHit World::raycast(const Vector3<float>& _origin,
                              const Vector3<float>& _direction,
                              const float _maxDistance,
                              const int _group,
                              const int _mask) const {
        auto termin = _origin + _direction.normalize() * _maxDistance;
        btCollisionWorld::ClosestRayResultCallback callback(mx_bt_cast(_origin), mx_bt_cast(termin));
        callback.m_collisionFilterGroup = _group;
        callback.m_collisionFilterMask = _mask;
        mWorld->rayTest(callback.m_rayFromWorld, callback.m_rayToWorld, callback);
        if(callback.hasHit())
            if(auto btrb = btRigidBody::upcast(callback.m_collisionObject)) {
                auto rb = static_scene_object_cast<RigidBody>(static_cast<RigidBody*>(btrb->getUserPointer())->getHandle());
                auto& point = callback.m_hitPointWorld;
                return RaycastHit(rb, bt_mx_cast(point));
            }
        return RaycastHit(nullptr, Vector3<float>(0, 0, 0));
    }

    std::vector<RaycastHit> World::raycastAll(const Vector3<float>& _origin,
                                              const Vector3<float>& _direction,
                                              const float _maxDistance) const {
        btCollisionWorld::AllHitsRayResultCallback callback({}, {});
        return raycastAll(_origin, _direction, _maxDistance, callback.m_collisionFilterGroup, callback.m_collisionFilterMask);
    }

    std::vector<RaycastHit> World::raycastAll(const Vector3<float>& _origin,
                                              const Vector3<float>& _direction,
                                              const float _maxDistance,
                                              const int _group,
                                              const int _mask) const {
        auto termin = _origin + _direction.normalize() * _maxDistance;
        btCollisionWorld::AllHitsRayResultCallback callback(mx_bt_cast(_origin), mx_bt_cast(termin));
        callback.m_collisionFilterGroup = _group;
        callback.m_collisionFilterMask = _mask;
        mWorld->rayTest(callback.m_rayFromWorld, callback.m_rayToWorld, callback);
        auto size = callback.m_collisionObjects.size();
        std::vector<RaycastHit> results;
        for(auto i = 0; i < size; ++i)
            if(auto btrb = btRigidBody::upcast(callback.m_collisionObjects[i])) {
                auto rb = static_scene_object_cast<RigidBody>(static_cast<RigidBody*>(btrb->getUserPointer())->getHandle());
                auto& point = callback.m_hitPointWorld[i];
                results.emplace_back(rb, bt_mx_cast(point));
            }
        return results;
    }

    World* World::Get() { return MixEngine::Instance().getModule<World>(); }

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

#pragma once

#ifndef MX_PHYSICS_WORLD_H_
#define MX_PHYSICS_WORLD_H_

#include "../Engine/MxModuleBase.h"
#include <bullet3/btBulletDynamicsCommon.h>
#include <set>

namespace Mix {
    template<typename _Ty>
    class Vector3;
    class RigidBody;

    namespace Physics {

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
        class DebugDraw;
#endif

        struct RaycastHit;

        class World final : public ModuleBase {
        public:
            World();

            ~World();

            void load() override;

            void init() override {}

            /**
             *  @brief This function steps simulation.
             *  @param _fixedDeltaTime Please pass Time::FixedDeltaTime()
             *  @note Must be called in fixed time step for optimization and accuracy.
             */
            void step(const btScalar _fixedDeltaTime);

            /**
             *  @brief Sync graphic transform with rigid body transform.
             *  @param _fixedDeltaTime Should be Time::FixedDeltaTime()
             *  @param _smoothing Should be Time::SmoothingFactor()
             *  @note Call this function in update()
             */
            void sync(const btScalar _fixedDeltaTime, const btScalar _smoothing) const;

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
            /**
             *  @brief Push draw data to render pipeline. See Physics::DebugDraw::drawLine()
             *  @note Should be called according to physics frame rate.
             */
            void pushDrawData() const;

            /**  @note Should be called according to physics frame rate. */
            void render() const;
#endif

            void theWorld() noexcept { mIsPaused = !mIsPaused; }

            RaycastHit raycast(const Vector3<float>& _origin,
                               const Vector3<float>& _direction,
                               const float _maxDistance) const;

            RaycastHit raycast(const Vector3<float>& _origin,
                               const Vector3<float>& _direction,
                               const float _maxDistance,
                               const int _group,
                               const int _mask) const;

            std::vector<RaycastHit> raycastAll(const Vector3<float>& _origin,
                                               const Vector3<float>& _direction,
                                               const float _maxDistance) const;

            std::vector<RaycastHit> raycastAll(const Vector3<float>& _origin,
                                               const Vector3<float>& _direction,
                                               const float _maxDistance,
                                               const int _group,
                                               const int _mask) const;

            auto getWorld() const noexcept { return mWorld; }

            static World* Get();

        private:
            using CollisionPair = std::pair<const btCollisionObject*, const btCollisionObject*>;
            using CollisionSet = std::set<CollisionPair>;

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_
            DebugDraw* mDebugDraw;
#endif

            btCollisionConfiguration* mConfig;
            btDispatcher* mDispatcher;
            btBroadphaseInterface* mBroadphase;
            btConstraintSolver* mSolver;
            btDiscreteDynamicsWorld* mWorld;

            bool mIsPaused;

            CollisionSet prevSet;

            void processCollisions();
        };
    }
}

#endif

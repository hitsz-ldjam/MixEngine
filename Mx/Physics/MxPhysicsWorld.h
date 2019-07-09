#pragma once

#ifndef MX_PHYSICS_WORLD_H_
#define MX_PHYSICS_WORLD_H_

#include "../Engine/MxModuleBase.h"
#include <bullet3/btBulletDynamicsCommon.h>

namespace Mix::Physics {
    class World final : public ModuleBase {
    public:
        World();
        ~World();
        void awake();
        /**
         *  @brief This function steps simulation.
         *  @param _fixedDeltaTime Please pass Mix::Time::FixedDeltaTime()
         *  @note Must be called in fixed time step for optimization and accuracy.
         */
        void step(const btScalar _fixedDeltaTime);
        /**
         *  @brief Sync graphic transform with rigid body transform.
         *  @param _fixedDeltaTime Should be Time::FixedDeltaTime()
         *  @param _smoothing Should be Time::SmoothingFactor()
         *  @note Call this function in update()
         */
        void sync(const btScalar _fixedDeltaTime, const btScalar _smoothing);
        auto getWorld() const noexcept { return mWorld; }

    private:
        btCollisionConfiguration* mConfig;
        btDispatcher* mDispatcher;
        btBroadphaseInterface* mBroadphase;
        btConstraintSolver* mSolver;
        btDiscreteDynamicsWorld* mWorld;
    };
}

#endif

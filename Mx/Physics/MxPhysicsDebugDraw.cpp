#include "MxPhysicsDebugDraw.h"
#include "MxPhysicsDebugDrawImpl.hpp"

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_

namespace Mix::Physics {
    DebugDraw::~DebugDraw() {
        delete mHandler;
    }

    void DebugDraw::awake() {
        delete mHandler;
        mHandler = new DebugDrawImpl::GLHandler();
        mHandler->awake();
    }

    void DebugDraw::render() {
        mHandler->render();
    }

    void DebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
        mHandler->pushLine(Vector3f(from.x(), from.y(), from.z()),
                           Vector3f(to.x(), to.y(), to.z()),
                           Vector3f(color.x(), color.y(), color.z()));
    }

    void DebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
        drawLine(PointOnB, PointOnB + normalOnB * distance, color);
    }
}

#endif

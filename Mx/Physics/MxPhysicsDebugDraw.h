#pragma once

#ifndef MX_PHYSICS_DEBUG_DRAW_H_
#define MX_PHYSICS_DEBUG_DRAW_H_

#ifdef MX_ENABLE_PHYSICS_DEBUG_DRAW_

#include <bullet3/LinearMath/btIDebugDraw.h>

namespace Mix::Physics {
    namespace DebugDrawImpl {
        class GLHandler;
    }

    /** @note DO NOT use this class manually unless you know what you are doing. */
    class DebugDraw final : public btIDebugDraw {
    public:
        DebugDraw() : mDebugMode(DBG_NoDebug),
                      mHandler(nullptr) {}

        ~DebugDraw();

        void awake();

        void render();

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

        /** @brief Current implementation draws a line between two contact points. */
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

        /** @note Not implemented yet */
        void reportErrorWarning(const char* warningString) override {}

        /** @note Not implemented yet */
        void draw3dText(const btVector3& location, const char* textString) override {}

        /** @note See btIDebugDraw::DebugDrawModes */
        void setDebugMode(int debugMode) override { mDebugMode = debugMode; }

        /** @note See btIDebugDraw::DebugDrawModes */
        int getDebugMode() const override { return mDebugMode; }

        void toggleMode(const DebugDrawModes _debugMode) {
            if(mDebugMode & _debugMode)
                mDebugMode &= (~_debugMode);
            else
                mDebugMode |= _debugMode;
        }

    private:
        int mDebugMode; /**< See btIDebugDraw::DebugDrawModes */
        DebugDrawImpl::GLHandler* mHandler;
    };
}

#endif

#endif

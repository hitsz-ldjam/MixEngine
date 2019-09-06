#pragma once

#ifndef MX_RIGID_BODY_UTILS_HPP_
#define MX_RIGID_BODY_UTILS_HPP_

#include <bullet3/btBulletDynamicsCommon.h>
#include <functional>

namespace Mix::Physics {
    enum class RigidbodyInterpolation {
        NONE,
        INTERPOLATE,
        EXTRAPOLATE,
    };

    struct RigidBodyConstructionInfo {
        /** Zero mass creates a static rigid body (@code btCollisionObject::CF_STATIC_OBJECT @endcode) */
        btScalar mass;

        /** Can be cast from Transform by calling @code Physics::mixTransToBtTrans @endcode */
        btTransform startTrans;

        /**
         *  1) Should NOT be nullptr. Local inertia tensor is calculated via this shape.\n
         *  2) Reusing collision shapes is highly recommended.\n
         *  3) The life times of collision shapes (and their children shapes) are automatically managed by
         *  RigidBody. DO NOT (and there is no need to) delete shapes manually.
         */
        btCollisionShape* shape;

        /** Used in MotionState construction. */
        RigidbodyInterpolation interpolation = RigidbodyInterpolation::NONE;

        /** Used in MotionState construction. */
        btTransform centerOfMassOffset = btTransform::getIdentity();

        /** 
         *  This function is called after btRigidBody::btRigidBodyConstructionInfo is constructed in RigidBody
         *  construction.\n
         *  DO NOT overwrite m_mass, m_motionState, m_collisionShape, m_localInertia in this function.
         *  
         *  @b See: btRigidBody::btRigidBodyConstructionInfo
         */
        std::function<void(btRigidBody::btRigidBodyConstructionInfo&)> furtherSetup = [](auto&) {};

        explicit RigidBodyConstructionInfo(const btScalar _mass,
                                           const btTransform& _startTrans,
                                           btCollisionShape* _shape) : mass(_mass),
                                                                       startTrans(_startTrans),
                                                                       shape(_shape) {}
    };

    ATTRIBUTE_ALIGNED16(struct) MotionState final : btMotionState {
        BT_DECLARE_ALIGNED_ALLOCATOR();

        explicit MotionState(const btTransform& _startTrans,
                             const RigidbodyInterpolation _interpolation = RigidbodyInterpolation::NONE,
                             const btTransform& _centerOfMassOffset = btTransform::getIdentity())
            : mPrevTrans(_startTrans),
              mCurrTrans(_startTrans),
              mInterpolatedTrans(_startTrans),
              mComOffset(_centerOfMassOffset),
              mInterpolation(_interpolation) {}

        void getWorldTransform(btTransform& _centerOfMassWorldTrans) const override {
            _centerOfMassWorldTrans = mCurrTrans * mComOffset.inverse();
        }

        void setWorldTransform(const btTransform& _centerOfMassWorldTrans) override {
            mPrevTrans = mCurrTrans;
            mCurrTrans = _centerOfMassWorldTrans * mComOffset;
        }

        auto interpolatedTransform() const {
            return mInterpolatedTrans * mComOffset.inverse();
        }

        /**
         *  @brief Interpolates transform. Should be called manually once per
         *  frame (update) if transform interpolation is desired.
         *  @param _fixedDeltaTime Should be Time::FixedDeltaTime()
         *  @param _smoothing Should be Time::SmoothingFactor()
         *
         *  @b todo: debug
         */
        void calculateInterpolatedTransform(const btScalar _fixedDeltaTime, const btScalar _smoothing) {
            switch(mInterpolation) {
                case RigidbodyInterpolation::NONE:
                {
                    mInterpolatedTrans = mCurrTrans;
                    break;
                }

                case RigidbodyInterpolation::INTERPOLATE:
                {
                    auto pos = mCurrTrans.getOrigin() * _smoothing
                               + mPrevTrans.getOrigin() * (1 - _smoothing);
                    auto rot = mCurrTrans.getRotation() * _smoothing
                               + mPrevTrans.getRotation() * (1 - _smoothing);
                    mInterpolatedTrans.setOrigin(pos);
                    mInterpolatedTrans.setRotation(rot);
                    break;
                }

                case RigidbodyInterpolation::EXTRAPOLATE:
                {
                    //auto linearVel = (mCurrTrans.getOrigin() - mPrevTrans.getOrigin()) / _fixedDeltaTime;
                    //auto angularVel = (mCurrTrans.getRotation() - mPrevTrans.getRotation()) / _fixedDeltaTime;
                    //auto pos = mCurrTrans.getOrigin() + linearVel * _soomthing * _fixedDeltaTime;
                    //auto rot = mCurrTrans.getRotation() + angularVel * _soomthing * _fixedDeltaTime;
                    //mInterpolatedTrans.setOrigin(pos);
                    //mInterpolatedTrans.setRotation(rot);

                    btVector3 linearVel, angularVel;
                    btTransformUtil::calculateVelocity(mPrevTrans,
                                                       mCurrTrans,
                                                       _smoothing * _fixedDeltaTime,
                                                       linearVel,
                                                       angularVel);
                    btTransformUtil::integrateTransform(mCurrTrans,
                                                        linearVel,
                                                        angularVel,
                                                        _smoothing * _fixedDeltaTime,
                                                        mInterpolatedTrans);

                    break;
                }

                default:
                    break;
            }
        }

    private:
        btTransform mPrevTrans, mCurrTrans, mInterpolatedTrans, mComOffset;
        RigidbodyInterpolation mInterpolation;
    };
}

#endif

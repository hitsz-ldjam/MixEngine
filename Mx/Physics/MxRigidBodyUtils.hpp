#pragma once

#ifndef MX_RIGID_BODY_UTILS_HPP_
#define MX_RIGID_BODY_UTILS_HPP_

#include <bullet3/btBulletDynamicsCommon.h>
#include <functional>

namespace Mix::Physics {
    enum class RigidbodyInterpolation {
        None,
        Interpolate,
        Extrapolate,
    };

    struct RigidBodyConstructionInfo {
        /** Zero mass creates a static rigid body i.e. btCollisionObject::CF_STATIC_OBJECT */
        btScalar mass;

        /** Can be cast from Transform by using @code Physics::mx_bt_cast @endcode */
        btTransform startTrans;

        /** The lifetime is NOT managed internally. */
        btCollisionShape* shape;

        /** Used in the construction of MotionState. */
        RigidbodyInterpolation interpolation = RigidbodyInterpolation::None;

        /** Used in the construction of MotionState. */
        btTransform centerOfMassOffset = btTransform::getIdentity();

        /** 
         *  This function is called after btRigidBody::btRigidBodyConstructionInfo is constructed in RigidBody
         *  construction.\n
         *  DO NOT overwrite m_mass, m_motionState, m_collisionShape, m_localInertia in this function unless
         *  you know what you are doing.
         *  @see btRigidBody::btRigidBodyConstructionInfo
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
                             const RigidbodyInterpolation _interpolation = RigidbodyInterpolation::None,
                             const btTransform& _centerOfMassOffset = btTransform::getIdentity())
            : mPrevTrans(_startTrans),
              mCurrTrans(_startTrans),
              mInterpolatedTrans(_startTrans),
              mComOffset(_centerOfMassOffset),
              mInterpolation(_interpolation) {}

        const auto& interpolation() const noexcept { return mInterpolation; }

        btTransform getWorldTransform() const {
            return mCurrTrans * mComOffset.inverse();
        }

        void getWorldTransform(btTransform& _centerOfMassWorldTrans) const override {
            _centerOfMassWorldTrans = getWorldTransform();
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
         *  @todo debug
         */
        void calculateInterpolatedTransform(const btScalar _fixedDeltaTime, const btScalar _smoothing) {
            switch(mInterpolation) {
                case RigidbodyInterpolation::None:
                {
                    mInterpolatedTrans = mCurrTrans;
                    break;
                }

                case RigidbodyInterpolation::Interpolate:
                {
                    auto pos = mCurrTrans.getOrigin() * _smoothing
                               + mPrevTrans.getOrigin() * (1 - _smoothing);
                    auto rot = mCurrTrans.getRotation() * _smoothing
                               + mPrevTrans.getRotation() * (1 - _smoothing);
                    mInterpolatedTrans.setOrigin(pos);
                    mInterpolatedTrans.setRotation(rot);
                    break;
                }

                case RigidbodyInterpolation::Extrapolate:
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

#pragma once

#ifndef MX_PHYSICS_HPP_
#define MX_PHYSICS_HPP_

#include <bullet3/btBulletDynamicsCommon.h>
#include "../Component/Transform/MxTransform.h"

namespace Mix::Physics {
    enum class RigidbodyInterpolation {
        NONE,
        INTERPOLATE,
        EXTRAPOLATE,
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
         *  frame(update) if interpolated transform is desired.
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

    // todo: debug. bullet uses right hand coordinate system!
    inline Transform btTransformToMixTransform(const btTransform& _btTrans) noexcept {
        Transform trans;
        trans.setPosition({
            _btTrans.getOrigin().x(),
            _btTrans.getOrigin().y(),
            _btTrans.getOrigin().z()
        });
        trans.setRotation({
            _btTrans.getRotation().w(),
            _btTrans.getRotation().x(),
            _btTrans.getRotation().y(),
            _btTrans.getRotation().z()
        });
        return trans;
    }

    inline btTransform mixTransformToBtTransform(const Transform& _mixTrans) noexcept {
        return btTransform({
                               _mixTrans.getRotation().x,
                               _mixTrans.getRotation().y,
                               _mixTrans.getRotation().z,
                               _mixTrans.getRotation().w
                           },
                           {
                               _mixTrans.getPosition().x,
                               _mixTrans.getPosition().y,
                               _mixTrans.getPosition().z
                           });
    }
}

#endif

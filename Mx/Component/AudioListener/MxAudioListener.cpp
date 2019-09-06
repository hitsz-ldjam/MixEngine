#include "MxAudioListener.h"

#include "../../GameObject/MxGameObject.h"
#include "../RigidBody/MxRigidBody.h"
#include "../../Time/MxTime.h"
#include "../../../MixEngine.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioListener, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioListener)

    void AudioListener::init() {
        if(mCore) return;
        if(!mGameObject) throw IndependentComponentError(getTypeName());
        mCore = MixEngine::Instance().getModule<Audio::Core>()->getCore();
        mLastPos = mGameObject->transform().getPosition();
        mUseFixedUpdate = mVelocityUpdateMode == Audio::VelocityUpdateMode::FIXED ||
                          mVelocityUpdateMode == Audio::VelocityUpdateMode::AUTO && mGameObject->getComponent<RigidBody>();
    }

    void AudioListener::fixedUpdate() {
        if(!mUseFixedUpdate) return;

        if(const auto* rb = mGameObject->getComponent<RigidBody>()) {
            const auto& pos = rb->get().getCenterOfMassPosition();
            const auto& vel = rb->get().getLinearVelocity();
            updatePosAndVel({pos.x(), pos.y(), pos.z()}, {vel.x(), vel.y(), vel.z()});
        }
        else {
            auto pos = mGameObject->transform().getPosition();
            auto vel = (pos - mLastPos) / Time::FixedDeltaTime();
            updatePosAndVel(pos, vel);
        }
    }

    void AudioListener::lateUpdate() {
        if(mUseFixedUpdate) return;

        auto pos = mGameObject->transform().getPosition();
        auto vel = (pos - mLastPos) / Time::DeltaTime();
        updatePosAndVel(pos, vel);
    }

    void AudioListener::updatePosAndVel(const Math::Vector3f& _pos, const Math::Vector3f& _vel) {
        const auto& trans = mGameObject->transform();

        auto glmVecToFmodVec = [](const Math::Vector3f& _vec) { return FMOD_VECTOR{_vec.x, _vec.y, _vec.z}; };

        auto fvPos = glmVecToFmodVec(_pos),
             fvVel = glmVecToFmodVec(_vel),
             fvForward = glmVecToFmodVec(trans.forward()),
             fvUp = glmVecToFmodVec(trans.up());

        if(mCore)
            mCore->set3DListenerAttributes(listenerIdx, &fvPos, &fvVel, &fvForward, &fvUp);

        mLastPos = _pos;
    }
}

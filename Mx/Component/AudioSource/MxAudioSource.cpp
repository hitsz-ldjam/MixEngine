#include "MxAudioSource.h"

#include "../../GameObject/MxGameObject.h"
#include "../../Time/MxTime.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioSource, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioSource)

    void AudioSource::play() {
        if(!mClip || mClip->loadState() != Audio::LoadState::LOADED)
            return;
        FMOD::System* core = nullptr;
        mClip->mSound->getSystemObject(&core);
        core->playSound(mClip->mSound, nullptr, false, &mChannel);
    }

    void AudioSource::init() {
        if(!mClip)
            return;

        // Field preloadAudioData is always true in order to make it simple.
        // if(mClip->preloadAudioData)

        if(mClip->loadAudioData() != FMOD_OK)
            throw FileLoadingError(mClip->mPath);

        FMOD::System* core = nullptr;
        mClip->mSound->getSystemObject(&core);
        if(mClip->loadState() == Audio::LoadState::LOADED)
            core->playSound(mClip->mSound, nullptr, !playOnAwake, &mChannel);

        if(!mGameObject)
            throw IndependentComponentError(getTypeName());
        mLastPos = mGameObject->transform().position();
        mUseFixedUpdate = velocityUpdateMode == Audio::VelocityUpdateMode::FIXED ||
                          velocityUpdateMode == Audio::VelocityUpdateMode::AUTO /*&& mGameObject->getComponent<Rigidbody>()*/;
    }

    void AudioSource::lateUpdate() {
        if(!mUseFixedUpdate)
            updatePosAndVel(Time::DeltaTime());
    }

    void AudioSource::fixedUpdate() {
        if(mUseFixedUpdate)
            updatePosAndVel(/*Time::FixedDeltaTime()*/ Time::DeltaTime());
    }

    void AudioSource::updatePosAndVel(const float _deltaTime) {
        auto pos = mGameObject->transform().position();
        auto vel = (pos - mLastPos) / _deltaTime;
        mLastPos = pos;

        auto fvPos = Audio::glmVec3ToFmodVec(pos);
        auto fvVel = Audio::glmVec3ToFmodVec(vel);

        if(mChannel)
            mChannel->set3DAttributes(&fvPos, &fvVel);
    }
}
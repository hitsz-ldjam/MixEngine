#include "MxAudioSource.h"

#include "../../GameObject/MxGameObject.h"
#include "../RigidBody/MxRigidBody.h"
#include "../../Time/MxTime.h"

namespace Mix {
    MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(AudioSource, Behaviour)
    MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(AudioSource)

    void AudioSource::dopplerLevel(const float _dopplerLevel) {
        if(mChannel) {
            mChannel->set3DDopplerLevel(_dopplerLevel);
            mChannel->get3DDopplerLevel(&mDopplerLevel);
        }
        else {
            mDopplerLevel = _dopplerLevel;
        }
    }

    bool AudioSource::isPlaying() const {
        bool is = false;
        if(mChannel)
            mChannel->isPlaying(&is);
        return is;
    }

    bool AudioSource::isVirtual() const {
        bool iv = true;
        if(mChannel)
            mChannel->isVirtual(&iv);
        return iv;
    }

    void AudioSource::loop(const bool _loop) {
        if(mChannel) {
            mChannel->setMode(_loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
            FMOD_MODE mode;
            mChannel->getMode(&mode);
            mLoop = mode & FMOD_LOOP_NORMAL;
        }
        else {
            mLoop = _loop;
        }
    }

    void AudioSource::mute(const bool _mute) {
        if(mChannel) {
            mChannel->setMute(_mute);
            mChannel->getMute(&mMute);
        }
        else {
            mMute = _mute;
        }
    }

    void AudioSource::pan(const float _pan) {
        if(mChannel) {
            mChannel->setPan(_pan);
        }
        else {
            mPan = _pan;
        }
    }

    void AudioSource::pitch(const float _pitch) {
        if(mChannel) {
            mChannel->setPitch(_pitch);
            mChannel->getPitch(&mPitch);
        }
        else {
            mPitch = _pitch;
        }
    }

    void AudioSource::priority(const int _priority) {
        if(mChannel) {
            mChannel->setPriority(_priority);
            mChannel->getPriority(&mPriority);
        }
        else {
            mPriority = _priority;
        }
    }

    void AudioSource::volume(const float _volume) {
        if(mChannel) {
            mChannel->setVolume(_volume);
            mChannel->getVolume(&mVolume);
        }
        else {
            mVolume = _volume;
        }
    }

    void AudioSource::play() {
        if(!mClip || mClip->loadState() != Audio::LoadState::LOADED)
            return;
        stop();
        FMOD::System* core = nullptr;
        mClip->mSound->getSystemObject(&core);
        core->playSound(mClip->mSound, nullptr, false, &mChannel);
        initChannelParameters();
    }

    void AudioSource::init() {
        if(!mGameObject)
            throw IndependentComponentError(getTypeName());

        play();
        if(!mPlayOnAwake)
            pause();
    }

    void AudioSource::fixedUpdate() {
        if(!mChannel || !mUseFixedUpdate) return;

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

    void AudioSource::lateUpdate() {
        if(!mChannel || mUseFixedUpdate) return;

        auto pos = mGameObject->transform().getPosition();
        auto vel = (pos - mLastPos) / Time::DeltaTime();
        updatePosAndVel(pos, vel);
    }

    void AudioSource::initChannelParameters() {
        if(!mChannel) return;

        mLastPos = mGameObject->transform().getPosition();
        mUseFixedUpdate = mVelocityUpdateMode == Audio::VelocityUpdateMode::FIXED ||
                          mVelocityUpdateMode == Audio::VelocityUpdateMode::AUTO && mGameObject->getComponent<RigidBody>();

        dopplerLevel(mDopplerLevel);
        loop(mLoop);
        mute(mMute);
        pan(mPan);
        pitch(mPitch);
        priority(mPriority);
        volume(mVolume);
    }

    void AudioSource::updatePosAndVel(const Math::Vector3f& _pos, const Math::Vector3f& _vel) {
        auto glmVecToFmodVec = [](const Math::Vector3f& _vec) { return FMOD_VECTOR{_vec.x, _vec.y, _vec.z}; };
        auto fvPos = glmVecToFmodVec(_pos),
             fvVel = glmVecToFmodVec(_vel);
        mChannel->set3DAttributes(&fvPos, &fvVel);
        mLastPos = _pos;
    }
}

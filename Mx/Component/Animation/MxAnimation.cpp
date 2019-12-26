#include "MxAnimation.h"

namespace Mix {
    MX_IMPLEMENT_RTTI(Animation, Component);

    Animation::Animation() {
        mInternal=AnimationImpl::Create();
    }

    void Animation::addClip(const std::string& _name, const std::shared_ptr<AnimationClip>& _clip) {
        mInternal->addClip(_name, _clip);
    }

    std::shared_ptr<AnimationClip> Animation::getClip(const std::string& _name) const {
        return mInternal->getClip(_name);
    }

    void Animation::removeClip(const std::string& _name) {
        mInternal->removeClip(_name);
    }

    uint32 Animation::getClipCount() const {
        return mInternal->getClipCount();
    }

    void Animation::setGlobalWrapMode(AnimWrapMode _wrapMode) {
        mInternal->setGlobalWrapMode(_wrapMode);
    }

    AnimWrapMode Animation::getGlobalWrapMode() const {
        return mInternal->getGlobalWrapMode();
    }

    void Animation::setGlobalSpeed(float _speed) {
        mInternal->setGlobalSpeed(_speed);
    }

    float Animation::getGlobalSpeed() const {
        return mInternal->getGlobalSpeed();
    }

    void Animation::enableGlobalOverride(bool _enbale) {
        mInternal->enableGlobalOverride(_enbale);
    }

    void Animation::play(const std::string& _name) {
        mInternal->play(_name);
    }

    bool Animation::isPlaying() const {
        return mInternal->isPlaying();
    }

    void Animation::stop() {
        return mInternal->stop();
    }

    std::shared_ptr<AnimationState> Animation::getState(const std::string& _name) const {
        return mInternal->getState(_name);
    }
}

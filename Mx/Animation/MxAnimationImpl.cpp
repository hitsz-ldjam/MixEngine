#include "MxAnimationImpl.h"
#include "../Math/MxMath.h"
#include "MxAnimationClip.h"
#include "MxSkeleton.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
    float AnimationState::getLength() const {
        return mClip->length();
    }

    void AnimationState::setNormalizedSpeed(float _nSpeed) {
        _nSpeed = Math::Wrap(_nSpeed, -1.0f, 1.0f);
        mSpeed = _nSpeed * getLength();
    }

    void AnimationState::setNormalizedTime(float _nTime) {
        _nTime = Math::Wrap(_nTime, 0.0f, 1.0f);
        mTime = _nTime * getLength();
    }

    void AnimationImpl::setSkeleton(const std::shared_ptr<Skeleton>& _skeleton) {
        if (mSkeleton != _skeleton) {
            mSkeleton = _skeleton;
            mDirtyFlags.set(AnimDirtyFlag::Skeleton);
        }
    }

    void AnimationImpl::mapToGameObjectByName(const HGameObject& _gameObject) {
        if (!mSkeleton)
            return;

        mGOBoneMap.clear();

        mapRootGameObject(_gameObject);
        auto children = _gameObject->getAllChildren(true);

        for (auto& entry : children) {
            auto index = mSkeleton->getBoneIndex(entry->getName());
            if (index != uint32(-1))
                mGOBoneMap[index] = GameObjectBoneMapInfo{ index, entry };
        }
    }

    void AnimationImpl::mapBoneToGameObject(const std::string& _name, const HGameObject& _gameObject) {
        if (!mSkeleton)
            return;

        auto index = mSkeleton->getBoneIndex(_name);
        if (index != uint32(-1))
            mGOBoneMap[_gameObject.getInstanceId()] = GameObjectBoneMapInfo{ index, _gameObject };
    }

    void AnimationImpl::unmapBoneGameObject(const HGameObject& _gameObject) {
        mGOBoneMap.erase(_gameObject.getInstanceId());
    }

    void AnimationImpl::mapRootGameObject(const HGameObject& _gameObject) {
        mRootGO = _gameObject;
    }

    void AnimationImpl::unmapRootGameObject() {
        mRootGO = nullptr;
    }

    void AnimationImpl::setGlobalWrapMode(AnimWrapMode _wrapMode) {
        mWrapMode = _wrapMode;
    }

    void AnimationImpl::setGlobalSpeed(float _speed) {
        mSpeed = _speed;
    }

    void AnimationImpl::addClip(const std::string& _name, const std::shared_ptr<AnimationClip>& _clip) {
        if (!_clip)
            return;

        auto it = mClipStates.find(_name);
        if (it != mClipStates.end())
            it->second->setClip(_clip);

        std::shared_ptr<std::vector<BoneCurveMap>> boneCurveMapping;

        mClipStates[_name] = std::shared_ptr<AnimationState>(new AnimationState(_name, _clip, 0.0f, DefaultSpeed, DefaultWrapMode));
    }

    std::shared_ptr<AnimationClip> AnimationImpl::getClip(const std::string& _name) const {
        auto it = mClipStates.find(_name);
        if (it != mClipStates.end())
            return it->second->getClip();
        return nullptr;
    }

    void AnimationImpl::removeClip(const std::string& _name) {
        mClipStates.erase(_name);
    }

    uint32 AnimationImpl::getClipCount() const {
        return mClipStates.size();
    }

    void AnimationImpl::play(const std::string& _name) {
        auto it = mClipStates.find(_name);
        if (it != mClipStates.end() && it->second != mCurrPlaying) {
            mCurrPlaying = it->second;
            mDirtyFlags.set(AnimDirtyFlag::Clip);
        }
    }

    bool AnimationImpl::isPlaying() const {
        return mCurrPlaying != nullptr;
    }

    void AnimationImpl::stop() {
        mCurrPlaying = nullptr;
    }

    std::shared_ptr<AnimationState> AnimationImpl::getState(const std::string& _name) const {
        auto it = mClipStates.find(_name);
        if (it != mClipStates.end())
            return it->second;
        return nullptr;
    }

    std::shared_ptr<AnimationImpl> AnimationImpl::Create() {
        return std::shared_ptr<AnimationImpl>(new AnimationImpl);
    }

    AnimationImpl::AnimationImpl() :mUpdateData(std::make_shared<AnimUpdateData>()) {
    }

    void AnimationImpl::prepareUpdateData(float _deltaTime) {
        if (mDirtyFlags & AnimDirtyFlag::Skeleton)
            if (mSkeleton != nullptr) {
                mUpdateData->skeleton = mSkeleton;
                mUpdateData->localPose.resize(mSkeleton->boneCount());
                mUpdateData->boneCurveMaps.resize(mSkeleton->boneCount());

                auto curves = mCurrPlaying->getClip()->getCurves();
                curves->getBoneCurveMaps(*mSkeleton, mUpdateData->boneCurveMaps.data());
            }
            else {
                mUpdateData->boneCurveMaps.clear();
            }

        if (mDirtyFlags.isSet(AnimDirtyFlag::Clip) && !mDirtyFlags.isSet(AnimDirtyFlag::Skeleton)) {
            auto curves = mCurrPlaying->getClip()->getCurves();
            curves->getBoneCurveMaps(*mSkeleton, mUpdateData->boneCurveMaps.data());
        }

        mUpdateData->state = mCurrPlaying;
        mDirtyFlags = AnimDirtyFlag::Clean;
    }

    void AnimationImpl::updateFromData() {
        for (auto& pair : mGOBoneMap) {
            if (pair.second.boneIndex == uint32(-1))
                continue;

            auto& trs = pair.second.gameObject->transform();
            auto& pose = mUpdateData->localPose[pair.second.boneIndex];
            trs.setLocalPosition(pose.position);
            trs.setLocalRotation(pose.rotation);
            trs.setLocalScale(pose.scale);
        }
    }
}

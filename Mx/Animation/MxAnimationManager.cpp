#include "MxAnimationManager.h"
#include "../Time/MxTime.h"
#include "../Scene/MxScene.h"
#include "../Component/Animation/MxAnimation.h"
#include "MxSkeleton.h"
#include "../../MixEngine.h"

namespace Mix {
    AnimationManager* AnimationManager::Get() {
        return MixEngine::Instance().getModule<AnimationManager>();
    }

    AnimationManager::AnimationManager() {
        mFrameAnimInfos.resize(SwapBufferCount);
    }

    FrameAnimInfo* AnimationManager::update(FrameSceneInfo& _sceneInfo) {
        if (mPaused) // Do nothing
            return &mFrameAnimInfos[mReadIdx];

        float deltaTime;
        if (!checkUpdateTime(deltaTime))
            return &mFrameAnimInfos[mReadIdx];

        std::vector<AnimUpdateData*> animData;
        for (auto anim : _sceneInfo.animations) {
            anim->mInternal->prepareUpdateData(deltaTime);
            animData.push_back(anim->mInternal->mUpdateData.get());
        }

        uint32 boneCount = 0;
        animData.reserve(_sceneInfo.animations.size());

        for (auto anim : _sceneInfo.animations) {
            if (animData.back()->skeleton != nullptr)
                boneCount += animData.back()->skeleton->boneCount();
        }

        auto& animInfo = mFrameAnimInfos[mWriteIdx];
        animInfo.animIndexMap.clear();
        animInfo.transforms.resize(boneCount);
        animInfo.animIndexMap.reserve(animData.size());

        {
            uint32 startIdx = 0;
            uint32 size = animData.size();
            for (uint32 i = 0; i < size; ++i) {
                evaluateAnimation(_sceneInfo.animations[i], *animData[i], startIdx);

                if (animData[i]->skeleton != nullptr)
                    startIdx += animData[i]->skeleton->boneCount();
            }
        }

        for (auto anim : _sceneInfo.animations)
            anim->mInternal->updateFromData();

        swapBuffer();
    }

    void AnimationManager::evaluateAnimation(Animation* _animation, AnimUpdateData& _data, uint32 _boneStartIdx) {
        // TODO do culling here

        auto& frameInfo = mFrameAnimInfos[mWriteIdx];

        FrameAnimInfo::AnimInfo animInfo;

        // Skeleton animation
        if (_data.skeleton != nullptr) {
            auto boneCount = _data.skeleton->boneCount();

            auto& poseInfo = animInfo.poseInfo;
            poseInfo.startIdx = _boneStartIdx;
            poseInfo.count = _data.skeleton->boneCount();

            Matrix4* buffer = frameInfo.transforms.data() + _boneStartIdx;

            _data.skeleton->getPose(buffer,
                                    _data.localPose.data(),
                                    _data.boneCurveMaps,
                                    *_data.state->getClip()->getCurves(),
                                    _data.state->getTime(),
                                    _data.state->getWrapMode() == AnimWrapMode::Clamp);
        }
        else {
            animInfo.poseInfo.startIdx = 0;
            animInfo.poseInfo.count = 0;
        }

        mFrameAnimInfos[mWriteIdx].animIndexMap[_animation] = animInfo;
    }

    void AnimationManager::setMaxSampleRate(uint32 _fps) {
        _fps = _fps == 0 ? 1 : _fps;

        mUpdateRate = 1.0f / _fps;
    }

    void AnimationManager::swapBuffer() {
        if (mSwap) {
            mReadIdx = mWriteIdx;
            mWriteIdx = (mWriteIdx + 1) % SwapBufferCount;

            mSwap = false;
        }
    }

    bool AnimationManager::checkUpdateTime(float& _delta) {
        mCurrAnimTime += Time::DeltaTime();
        bool result = mCurrAnimTime >= mNextUpdateTime;

        /*
         *
         * |<-rate->|
         * |________|________|________
         * |        |        |
         *          ^  ^     ^
         *          l  c     n
         */
        mNextUpdateTime = std::floor(mCurrAnimTime / mUpdateRate)*mUpdateRate + mUpdateRate;
        _delta = mCurrAnimTime - mLastUpdateTime;
        mLastUpdateTime = mCurrAnimTime;

        return result;
    }

}

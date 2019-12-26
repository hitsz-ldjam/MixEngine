#include "MxAnimationClip.h"
#include "MxSkeleton.h"

namespace Mix {
    AnimationCurveSet::AnimationCurveSet(const AnimationCurveMap& _map) {
        rebuild(_map);
    }

    void AnimationCurveSet::rebuild(const AnimationCurveMap& _map) {
        position.clear();
        rotation.clear();
        scale.clear();
        value.clear();
        nameIndexMap.clear();

        auto build = [&](const auto& _curves, uint32 _channelIdx) {
            uint32 curveIdx = 0;
            for (const auto& curve : _curves) {
                auto it = nameIndexMap.find(curve.first);
                if (it != nameIndexMap.end())
                    nameIndexMap[curve.first][_channelIdx] = curveIdx;
                else {
                    nameIndexMap[curve.first].fill(uint32(-1));
                    nameIndexMap[curve.first][_channelIdx] = curveIdx;
                }
            }
        };

        build(_map.position, uint32(AnimationChannel::Position));
        build(_map.rotation, uint32(AnimationChannel::Rotation));
        build(_map.scale, uint32(AnimationChannel::Position));
        build(_map.value, uint32(AnimationChannel::Value));
    }

    void AnimationCurveSet::getBoneCurveMaps(const Skeleton& _skeleton, BoneCurveMap* _maps) const {
        auto boneCount = _skeleton.boneCount();
        for (uint32 i = 0; i < boneCount; ++i) {
            auto it = nameIndexMap.find(_skeleton.getBoneInfo(i).name);
            if (it == nameIndexMap.end()) {
                _maps[i] = BoneCurveMap{ uint32(-1), uint32(-1), uint32(-1) };
            }
            else {
                _maps[i] = BoneCurveMap{
                    it->second[uint32(AnimationChannel::Position)],
                    it->second[uint32(AnimationChannel::Rotation)],
                    it->second[uint32(AnimationChannel::Scale)]
                };
            }
        }
    }

    void AnimationClip::setCurves(const AnimationCurveMap& _curves) {
        mCurves->rebuild(_curves);
        calculateLength();
    }

    bool AnimationClip::hasRootMotion() const {
        return mRootMotion != nullptr && mRootMotion->position.keyCount() || mRootMotion->rotation.keyCount();
    }

    std::shared_ptr<AnimationClip> AnimationClip::Create() {
        return std::shared_ptr<AnimationClip>(new AnimationClip());
    }

    std::shared_ptr<AnimationClip> AnimationClip::Create(const std::shared_ptr<AnimationCurveMap>& _curves, uint32 _sampleRate, const std::shared_ptr<RootMotion>& _rootMotion) {
        return std::shared_ptr<AnimationClip>(new AnimationClip(_curves, _sampleRate, _rootMotion));
    }

    AnimationClip::AnimationClip() {
        mCurves = std::make_shared<AnimationCurveSet>();
        mRootMotion = std::make_shared<RootMotion>();
    }

    AnimationClip::AnimationClip(const std::shared_ptr<AnimationCurveMap>& _curves, uint32 _sampleRate, const std::shared_ptr<RootMotion>& _rootMotion) : mSampleRate(_sampleRate) {
        mCurves = _curves == nullptr ? std::make_shared<AnimationCurveSet>() : std::make_shared<AnimationCurveSet>(*_curves);
        mRootMotion = _rootMotion == nullptr ? std::make_shared<RootMotion>() : _rootMotion;

    }

    void AnimationClip::calculateLength() {
        float length = 0.0f;

        for (auto& entry : mCurves->position)
            length = std::max(length, entry.length());

        for (auto& entry : mCurves->rotation)
            length = std::max(length, entry.length());

        for (auto& entry : mCurves->scale)
            length = std::max(length, entry.length());

        for (auto& entry : mCurves->value)
            length = std::max(length, entry.length());

        mLength = length;
    }
}

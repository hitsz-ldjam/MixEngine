#pragma once
#ifndef MX_ANIMATION_CLIP_H_
#define MX_ANIMATION_CLIP_H_
#include <memory>
#include "MxAnimationCurve.h"

namespace Mix {
    class Skeleton;

    enum class AnimationChannel {
        Position,
        Rotation,
        Scale,
        Value,

        Count
    };

    struct BoneCurveMap {
        uint32 position = uint32(-1);
        uint32 rotation = uint32(-1);
        uint32 scale = uint32(-1);
    };

    struct AnimationCurveSet {
        AnimationCurveSet() = default;

        explicit AnimationCurveSet(const AnimationCurveMap& _map);

        void rebuild(const AnimationCurveMap& _map);

        void getBoneCurveMaps(const Skeleton& _skeleton, BoneCurveMap* _maps) const;

        std::vector<AnimCurvePosition> position;
        std::vector<AnimCurveRotation> rotation;
        std::vector<AnimCurveScale> scale;
        std::vector<AnimCurveValue> value;

        std::unordered_map<std::string, std::array<uint32, uint32(AnimationChannel::Count)>> nameIndexMap;
    };

    class AnimationClip {
    public:
        std::shared_ptr<AnimationCurveSet> getCurves() const { return mCurves; }

        void setCurves(const AnimationCurveMap& _curves);

        std::shared_ptr<RootMotion> getRootMotion() const { return mRootMotion; }

        bool hasRootMotion() const;

        float length() const { return mLength; }

        uint32 getSampleRate() const { return mSampleRate; }

        void setSampleRate(uint32 _sampleRate) { mSampleRate = _sampleRate; }

        static std::shared_ptr<AnimationClip> Create();

        static std::shared_ptr<AnimationClip> Create(const std::shared_ptr<AnimationCurveMap>& _curves, uint32 _sampleRate, const std::shared_ptr<RootMotion>& _rootMotion);

    private:
        AnimationClip();

        AnimationClip(const std::shared_ptr<AnimationCurveMap>& _curves, uint32 _sampleRate,
                      const std::shared_ptr<RootMotion>& _rootMotion);

        void calculateLength();

        std::shared_ptr<AnimationCurveSet> mCurves;
        std::shared_ptr<RootMotion> mRootMotion;
        uint32 mSampleRate = 1;
        float mLength = 0.0f;
    };
}

#endif

#pragma once
#ifndef MX_ANIMATION_CURVE_H_
#define MX_ANIMATION_CURVE_H_
#include "../Math/MxVector3.h"
#include "../Math/MxQuaternion.h"
#include "../Utils/MxArrayProxy.h"
#include "../Definitions/MxTypes.h"

namespace Mix {
    template<typename _Ty>
    struct TKeyFrame {
        _Ty value;
        _Ty inTangent;
        _Ty outTangent;
        float time;

        bool operator==(const TKeyFrame& _other) const {
            return value == _other.value &&
                inTangent == _other.inTangent &&
                outTangent == _other.outTangent &&
                time == _other.time;
        }

        bool operator!=(const TKeyFrame& _other) const {
            return !(*this == _other);
        }
    };

    template struct TKeyFrame<float>;
    template struct TKeyFrame<Vector3f>;
    template struct TKeyFrame<Quaternion>;

    enum class AnimSampleType {
        Linear,
        Step,
        CubicSpline
    };

    template<typename _Ty>
    class TAnimationCurve {
    public:
        typedef TKeyFrame<_Ty> KeyFrame;

        TAnimationCurve() = default;

        explicit TAnimationCurve(InArrayProxy<KeyFrame> _keys, AnimSampleType _type);

        _Ty evaluate(float _time, bool _clamp = false) const {
            return evaluate(mType, _time, _clamp);
        }

        _Ty evaluate(AnimSampleType _type, float _time, bool _clamp = false) const;

        uint32 keyCount() const { return mKeyFrames.size(); }

        const KeyFrame& getKeyFrame(uint32 _idx) const { return mKeyFrames[_idx]; }

        const std::vector<KeyFrame>& getKeyFrames() const { return mKeyFrames; }

        float length() const { return mKeyFrames.empty() ? 0.0f : mKeyFrames.back().time; }

    private:
        static _Ty EvaluateLinear(float _time, const KeyFrame& _l, const KeyFrame& _r);

        static _Ty EvaluateStep(float _time, const KeyFrame& _l, const KeyFrame& _r);

        static _Ty EvaluateSpline(float _time, const KeyFrame& _l, const KeyFrame& _r);

        std::vector<KeyFrame> mKeyFrames;
        AnimSampleType mType = AnimSampleType::Linear;
    };

    template class TAnimationCurve<float>;
    template class TAnimationCurve<Vector3f>;
    template class TAnimationCurve<Quaternion>;

    template <typename _Ty>
    TAnimationCurve<_Ty>::TAnimationCurve(InArrayProxy<KeyFrame> _keys, AnimSampleType _type) :mType(_type) {
        mKeyFrames.reserve(_keys.size());
        auto size = _keys.size();
        for (uint32 i = 0; i < size; ++i)
            mKeyFrames.emplace_back(_keys[i]);
    }

    template <typename _Ty>
    _Ty TAnimationCurve<_Ty>::evaluate(AnimSampleType _type, float _time, bool _clamp) const {
        if (mKeyFrames.empty())
            return {};

        auto endTime = mKeyFrames.back().time;
        if (_clamp)
            _time = Math::Clamp(_time, 0.0f, endTime);
        else
            _time = Math::Wrap(_time, 0.0f, endTime);

        auto r = std::lower_bound(mKeyFrames.begin(), mKeyFrames.end(), _time, [](const KeyFrame& _e, float _v) {return _e.time <= _v; });

        if (r == mKeyFrames.begin())
            return r->value;

        decltype(r) l = r - 1;

        MX_ASSERT(r->time - l->time > 0.0f);

        switch (_type) {
        case AnimSampleType::Linear: return EvaluateLinear(_time, *l, *r);
        case AnimSampleType::Step: return EvaluateStep(_time, *l, *r);
        case AnimSampleType::CubicSpline: return EvaluateSpline(_time, *l, *r);
        default: return EvaluateSpline(_time, *l, *r);
        }
    }

    template <typename _Ty>
    _Ty TAnimationCurve<_Ty>::EvaluateLinear(float _time, const KeyFrame& _l, const KeyFrame& _r) {
        auto t = (_time - _l.time) / (_r.time - _l.time);
        return Math::Lerp(_l.value, _r.value, t);
    }

    template <typename _Ty>
    _Ty TAnimationCurve<_Ty>::EvaluateStep(float _time, const KeyFrame& _l, const KeyFrame& _r) {
        if (_time - _l.time > _r.time - _time)
            return _r.value;
        else
            return _l.value;
    }

    template <typename _Ty>
    _Ty TAnimationCurve<_Ty>::EvaluateSpline(float _time, const KeyFrame& _l, const KeyFrame& _r) {
        auto delta = _r.time - _l.time;

        float t;
        _Ty preTangent;
        _Ty nxtTangent;

        if (Math::EpsilonEqual(delta, 0.0f)) {
            t = 0.0f;
            preTangent = Math::Zero<_Ty>();
            nxtTangent = Math::Zero<_Ty>();
        }
        else {
            t = (_time - _l.time) / delta;
            preTangent = _l.outTangent * delta;
            nxtTangent = _r.inTangent * delta;
        }

        return Math::CubicSpline(t, _l.value, preTangent, _r.value, nxtTangent);
    }

    using AnimCurvePosition = TAnimationCurve<Vector3f>;
    using AnimCurveRotation = TAnimationCurve<Quaternion>;
    using AnimCurveScale = TAnimationCurve<Vector3f>;
    using AnimCurveValue = TAnimationCurve<float>;

    struct AnimationCurveMap {
        AnimationCurveMap() = default;

        void addPositionCurve(const std::string& _name, const AnimCurvePosition& _curve);

        void removePositionCurve(const std::string& _name);

        void addRotationCurve(const std::string& _name, const AnimCurveRotation& _curve);

        void removeRotationCurve(const std::string& _name);

        void addScaleCurve(const std::string& _name, const AnimCurveScale& _curve);

        void removeScaleCurve(const std::string& _name);

        void addValueCurve(const std::string& _name, const AnimCurveValue& _curve);

        void removeValueCurve(const std::string& _name);

        std::unordered_map<std::string, AnimCurvePosition> position;
        std::unordered_map<std::string, AnimCurveRotation> rotation;
        std::unordered_map<std::string, AnimCurveScale> scale;
        std::unordered_map<std::string, AnimCurveValue> value;
    };

    struct RootMotion {
        RootMotion() = default;

        RootMotion(const AnimCurvePosition& _position, const AnimCurveRotation& _rotation) :position(_position), rotation(_rotation) {}

        AnimCurvePosition position;
        AnimCurveRotation rotation;
    };
}

#endif

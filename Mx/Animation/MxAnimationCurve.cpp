#include "MxAnimationCurve.h"

namespace Mix {

    void AnimationCurveMap::addPositionCurve(const std::string& _name, const AnimCurvePosition& _curve) {
        position[_name] = _curve;
    }

    void AnimationCurveMap::removePositionCurve(const std::string& _name) {
        position.erase(_name);
    }

    void AnimationCurveMap::addRotationCurve(const std::string& _name, const AnimCurveRotation& _curve) {
        rotation[_name] = _curve;
    }

    void AnimationCurveMap::removeRotationCurve(const std::string& _name) {
        rotation.erase(_name);
    }

    void AnimationCurveMap::addScaleCurve(const std::string& _name, const AnimCurveScale& _curve) {
        scale[_name] = _curve;
    }

    void AnimationCurveMap::removeScaleCurve(const std::string& _name) {
        scale.erase(_name);
    }

    void AnimationCurveMap::addValueCurve(const std::string& _name, const AnimCurveValue& _curve) {
        value[_name] = _curve;
    }

    void AnimationCurveMap::removeValueCurve(const std::string& _name) {
        value.erase(_name);
    }
}

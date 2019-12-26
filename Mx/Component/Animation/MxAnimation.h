#pragma once
#ifndef MX_ANIMATION_H_
#define MX_ANIMATION_H_
#include "../MxComponent.h"
#include "../../Animation/MxAnimationImpl.h"

namespace Mix {
    class AnimationImpl;
    class AnimationClip;
    class AnimationManager;

    class Animation :public Component {
        MX_DECLARE_RTTI;
    public:
        Animation();

        void addClip(const std::string& _name, const std::shared_ptr<AnimationClip>& _clip);

        std::shared_ptr<AnimationClip> getClip(const std::string& _name) const;

        void removeClip(const std::string& _name);

        uint32 getClipCount() const;

        void setGlobalWrapMode(AnimWrapMode _wrapMode);

        AnimWrapMode getGlobalWrapMode() const;

        void setGlobalSpeed(float _speed);

        float getGlobalSpeed() const;

        void enableGlobalOverride(bool _enbale);

        void play(const std::string& _name);

        bool isPlaying() const;

        void stop();

        std::shared_ptr<AnimationState> getState(const std::string& _name) const;

    private:
        friend AnimationManager;

        std::shared_ptr<AnimationImpl> mInternal;
    };
}

#endif

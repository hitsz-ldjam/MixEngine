#pragma once
#ifndef MX_ANIMATION_IMPL_H_
#define MX_ANIMATION_IMPL_H_
#include <string>
#include "../Definitions/MxTypes.h"
#include <unordered_map>
#include <optional>
#include <utility>
#include "../Scene/MxSceneObjectManager.h"
#include "../Scene/MxSceneObjectHandle.h"
#include "../Math/MxTransformation.h"
#include "../Utils/MxFlags.h"
#include "MxAnimationClip.h"

namespace Mix {
    class AnimationClip;
    class Skeleton;
    class AnimationImpl;
    class AnimationManager;

    enum class AnimWrapMode {
        Loop,
        Clamp
    };

    class AnimationState {
    public:
        const std::string& getName() const { return mName; }

        std::shared_ptr<AnimationClip> getClip() const { return mClip; }

        void setClip(std::shared_ptr<AnimationClip> _clip) { mClip = std::move(_clip); }

        float getLength() const;

        float getSpeed() const { return mSpeed; }

        void setSpeed(float _speed) { mSpeed = _speed; }

        float getTime() const { return mTime; }

        void setTime(float _time) { mTime = _time; }

        float getNormalizedSpeed() const { return mSpeed / getLength(); }

        void setNormalizedSpeed(float _nSpeed);

        float getNormalizedTime() const { return mTime / getLength(); }

        void setNormalizedTime(float _nTime);

        AnimWrapMode getWrapMode() const { return mWrapMode; }

        void setWrapMode(AnimWrapMode _wrapMode) { mWrapMode = _wrapMode; }

    private:
        friend AnimationImpl;

        AnimationState(std::string _name, std::shared_ptr<AnimationClip> _clip, float _time, float _speed, AnimWrapMode _wrapMode) :
            mName(std::move(_name)),
            mClip(std::move(_clip)),
            mTime(_time),
            mSpeed(_speed),
            mWrapMode(_wrapMode) {
        }

        std::string mName;
        std::shared_ptr<AnimationClip> mClip;
        float mTime = 0.0f;
        float mSpeed = 1.0f;
        AnimWrapMode mWrapMode = AnimWrapMode::Loop;
    };

    /**
     * \brief Used to update animation.
     */
    struct AnimUpdateData {
        std::shared_ptr<AnimationState> state;
        std::shared_ptr<Skeleton> skeleton;
        std::vector<BoneCurveMap> boneCurveMaps;

        // Update result
        std::vector<TRS> localPose; /**< Local pose for GameObject */
    };

    enum class AnimDirtyFlag {
        Clean = 1u << 0,
        Skeleton = 1u << 1,
        Clip = 1u << 2,
    };

    MX_ALLOW_FLAGS_FOR_ENUM(AnimDirtyFlag);

    class AnimationImpl {
    public:
        ~AnimationImpl() = default;

        /////////////////////////////////////////////////
        // Skeleton
        /////////////////////////////////////////////////

        void setSkeleton(const std::shared_ptr<Skeleton>& _skeleton);

        std::shared_ptr<Skeleton> getSkeleton() const { return mSkeleton; }

        /////////////////////////////////////////////////
        // GameObject mapping
        /////////////////////////////////////////////////

        /**
         * \brief Map a game object to skeleton by name. The user should make sure that this game object has the correct hierarchy.
         */
        void mapToGameObjectByName(const HGameObject& _gameObject);

        void mapBoneToGameObject(const std::string& _name, const HGameObject& _gameObject);

        void unmapBoneGameObject(const HGameObject& _gameObject);

        void mapRootGameObject(const HGameObject& _gameObject);

        void unmapRootGameObject();

        /////////////////////////////////////////////////
        // Global playback parameter
        /////////////////////////////////////////////////

        void setGlobalWrapMode(AnimWrapMode _wrapMode);

        AnimWrapMode getGlobalWrapMode() const { return mWrapMode; }

        void setGlobalSpeed(float _speed);

        float getGlobalSpeed() const { return mSpeed; }

        void enableGlobalOverride(bool _enbale) { mGlobalOverride = _enbale; }

        /////////////////////////////////////////////////
        // Animation clip
        /////////////////////////////////////////////////

        void addClip(const std::string& _name, const std::shared_ptr<AnimationClip>& _clip);

        std::shared_ptr<AnimationClip> getClip(const std::string& _name) const;

        void removeClip(const std::string& _name);

        uint32 getClipCount() const;

        /////////////////////////////////////////////////
        // Playback
        /////////////////////////////////////////////////

        void play(const std::string& _name);

        bool isPlaying() const;

        void stop();

        /////////////////////////////////////////////////
        // Animation state
        /////////////////////////////////////////////////

        std::shared_ptr<AnimationState> getState(const std::string& _name) const;

        static constexpr float DefaultSpeed = 1.0f;

        static constexpr AnimWrapMode DefaultWrapMode = AnimWrapMode::Loop;

        static std::shared_ptr<AnimationImpl> Create();

    private:
        friend AnimationManager;

        AnimationImpl();

        void prepareUpdateData(float _deltaTime);

        void updateFromData();

        bool mGlobalOverride = false;
        AnimWrapMode mWrapMode = DefaultWrapMode;
        float mSpeed = DefaultSpeed;

        /////////////////////////////////////////////////
        // Skeleton
        /////////////////////////////////////////////////
        std::shared_ptr<AnimationState> mCurrPlaying;
        std::unordered_map<std::string, std::shared_ptr<AnimationState>> mClipStates;
        std::shared_ptr<Skeleton> mSkeleton;


        /////////////////////////////////////////////////
        // GameObject mapping
        /////////////////////////////////////////////////
        struct GameObjectBoneMapInfo {
            uint32 boneIndex = uint32(-1);
            HGameObject gameObject;
        };

        std::unordered_map<uint64, GameObjectBoneMapInfo> mGOBoneMap;
        HGameObject mRootGO;

        /////////////////////////////////////////////////
        // Update
        /////////////////////////////////////////////////
        std::shared_ptr<AnimUpdateData> mUpdateData;

        Flags<AnimDirtyFlag> mDirtyFlags = AnimDirtyFlag::Skeleton;
    };

}

#endif

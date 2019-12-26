#pragma once
#ifndef MX_ANIMATION_MANAGER_H_
#define MX_ANIMATION_MANAGER_H_
#include "../Engine/MxModuleBase.h"
#include "../Definitions/MxTypes.h"
#include "../Math/MxMatrix4.h"

namespace Mix {
    struct AnimUpdateData;
    struct FrameSceneInfo;
    class Animation;

    struct FrameAnimInfo {
        struct PoseIndexInfo {
            uint32 startIdx = 0;
            uint32 count = 0;
        };

        struct AnimInfo {
            PoseIndexInfo poseInfo;
            // TODO Add MorphShape
        };

        std::unordered_map<Animation*, AnimInfo> animIndexMap;
        std::vector<Matrix4> transforms;
    };

    class AnimationManager :public ModuleBase {
    public:
        static AnimationManager* Get();

        AnimationManager();

        void setPaused(bool _paused) { mPaused = _paused; }

        FrameAnimInfo* update(FrameSceneInfo& _sceneInfo);

        void setMaxSampleRate(uint32 _fps);

    private:
        static constexpr uint32 SwapBufferCount = 2;

        void swapBuffer();

        bool checkUpdateTime(float& _delta);

        void evaluateAnimation(Animation* _animation ,AnimUpdateData& _data, uint32 _boneStartIdx);

        bool mPaused = false;
        float mUpdateRate = 1.0f / 60.0f;
        float mCurrAnimTime = 0.0f;
        float mLastUpdateTime = 0.0f;
        float mNextUpdateTime = 0.0f;

        std::vector<FrameAnimInfo> mFrameAnimInfos;
        bool mSwap = false;
        uint32 mWriteIdx = 0;
        uint32 mReadIdx = 1;
    };
}

#endif

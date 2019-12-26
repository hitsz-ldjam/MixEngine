#include "MxSkeleton.h"
#include "MxAnimationClip.h"
#include <stack>

namespace Mix {

    Skeleton::~Skeleton() {
        delete[] mTransforms;
        delete[] mInvBindMats;
        delete[] mBoneInfos;
    }

    void Skeleton::getPose(Matrix4* _pose, TRS* _localPose, ArrayProxy<BoneCurveMap> _boneCurveMapping, const AnimationCurveSet& _curves, float _time, bool _clamp) {
        // We have sorted the indices before, now we just need to go through it once.
        for (uint32 i = mBoneCount - 1; i >= 0; --i) {
            auto idx = mSortedIdx[i];

            Vector3f position;
            Quaternion rotation;
            Vector3f scale(1.0f);

            auto channel = _boneCurveMapping[idx].position;
            if (channel != uint32(-1))
                position = _curves.position[channel].evaluate(_time, _clamp);

            channel = _boneCurveMapping[idx].rotation;
            if (channel != uint32(-1))
                rotation = _curves.rotation[channel].evaluate(_time, _clamp).normalize();

            channel = _boneCurveMapping[idx].scale;
            if (channel != uint32(-1))
                scale = _curves.scale[channel].evaluate(_time, _clamp);

            _pose[idx] = Matrix4::TRS(position, rotation, scale);

            if (mBoneInfos[idx].parent != uint32(-1))
                _pose[idx] = _pose[mBoneInfos[idx].parent] * _pose[idx];

            _localPose[idx] = TRS(position, rotation, scale);
        }
    }

    void Skeleton::getPose(Matrix4* _pose, ArrayProxy<BoneCurveMap> _boneCurveMapping, const AnimationCurveSet& _curves, float _time, bool _clamp) {
        // We have sorted the indices before, now we just need to go through it once.
        for (uint32 i = mBoneCount - 1; i >= 0; --i) {
            auto idx = mSortedIdx[i];

            Vector3f position;
            Quaternion rotation;
            Vector3f scale(1.0f);

            auto channel = _boneCurveMapping[idx].position;
            if (channel != uint32(-1))
                position = _curves.position[channel].evaluate(_time, _clamp);

            channel = _boneCurveMapping[idx].rotation;
            if (channel != uint32(-1))
                rotation = _curves.rotation[channel].evaluate(_time, _clamp).normalize();

            channel = _boneCurveMapping[idx].scale;
            if (channel != uint32(-1))
                scale = _curves.scale[channel].evaluate(_time, _clamp);

            _pose[idx] = Matrix4::TRS(position, rotation, scale);

            if (mBoneInfos[idx].parent != uint32(-1))
                _pose[idx] = _pose[mBoneInfos[idx].parent] * _pose[idx];
        }
    }

    void Skeleton::getPose(Matrix4* _pose, TRS* _localPose, const AnimationClip& _clip, float _time, bool _clamp) {
        auto curves = _clip.getCurves();
        std::vector<BoneCurveMap> boneCurveMaps(mBoneCount);
        curves->getBoneCurveMaps(*this, boneCurveMaps.data());

        getPose(_pose, _localPose, boneCurveMaps, *curves, _time, _clamp);
    }

    void Skeleton::getPose(Matrix4* _pose, const AnimationClip& _clip, float _time, bool _clamp) {
        auto curves = _clip.getCurves();
        std::vector<BoneCurveMap> boneCurveMaps(mBoneCount);
        curves->getBoneCurveMaps(*this, boneCurveMaps.data());

        getPose(_pose, boneCurveMaps, *curves, _time, _clamp);
    }

    uint32 Skeleton::getBoneIndex(const std::string& _name) {
        auto it = mNameIndexMap.find(_name);
        if (it != mNameIndexMap.end())
            return it->second;

        return uint32(-1);
    }

    std::shared_ptr<Skeleton> Skeleton::Create(ArrayProxy<BoneDesc> _bones) {
        return std::shared_ptr<Skeleton>(new Skeleton(_bones));
    }

    Skeleton::Skeleton(ArrayProxy<BoneDesc> _bones) :
        mBoneCount(_bones.size()),
        mTransforms(new TRS[_bones.size()]),
        mInvBindMats(new Matrix4[_bones.size()]),
        mBoneInfos(new BoneInfo[_bones.size()]),
        mSortedIdx(new uint32[_bones.size()]) {

        for (uint32_t i = 0; i < _bones.size(); ++i) {
            mTransforms[i] = { _bones[i].position, _bones[i].rotation, _bones[i].scale };
            mInvBindMats[i] = _bones[i].invBindMat;
            mBoneInfos[i].name = _bones[i].name;
            mBoneInfos[i].parent = _bones[i].parent;
        }

        sortIdx();
        buildMap();
    }

    void Skeleton::sortIdx() {
        std::vector<uint32> inDegrees(mBoneCount, 0);
        for (uint32 i = 0; i < mBoneCount; ++i) {
            if (mBoneInfos[i].parent != uint32(-1))
                ++inDegrees[mBoneInfos[i].parent];
        }

        std::stack<uint32> zeroDegree;

        for (uint32 i = 0; i < mBoneCount; ++i) {
            if (inDegrees[i] == 0)
                zeroDegree.push(i);
        }

        MX_ASSERT(!zeroDegree.empty() && "Bad skeleton");

        uint32 idx = 0;
        while (!zeroDegree.empty()) {
            mSortedIdx[idx] = zeroDegree.top();
            zeroDegree.pop();

            auto succs = mBoneInfos[mSortedIdx[idx]].parent;
            if (--inDegrees[succs] == 0)
                zeroDegree.push(succs);

            ++idx;
        }
    }

    void Skeleton::buildMap() {
        for (uint32 i = 0; i < mBoneCount; ++i)
            mNameIndexMap[mBoneInfos[i].name] = i;
    }
}

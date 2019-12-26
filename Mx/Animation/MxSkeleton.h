#pragma once
#ifndef MX_SKELETON_H_
#define MX_SKELETON_H_

#include <string>
#include <utility>
#include "../Math/MxTransformation.h"
#include "../Math/MxMatrix4.h"
#include "../Utils/MxArrayProxy.h"

namespace Mix {
    class AnimationClip;
    struct BoneCurveMap;
    struct AnimationCurveSet;

    struct BoneDesc {
        std::string name;
        uint32_t parent;
        Vector3f position;
        Quaternion rotation;
        Vector3f scale;
        Matrix4 invBindMat;
    };

    class Skeleton {
    public:
        struct BoneInfo {
            std::string name;
            uint32_t parent = uint32_t(-1);

            BoneInfo() = default;

            BoneInfo(std::string _name, uint32_t _parent) :name(std::move(_name)), parent(_parent) {}
        };

        ~Skeleton();

        void getPose(Matrix4* _pose, TRS* _localPose, ArrayProxy<BoneCurveMap> _boneCurveMapping, const AnimationCurveSet& _curves, float _time, bool _clamp);

        void getPose(Matrix4* _pose, ArrayProxy<BoneCurveMap> _boneCurveMapping, const AnimationCurveSet& _curves, float _time, bool _clamp);

        void getPose(Matrix4* _pose, TRS* _localPose, const AnimationClip& _clip, float _time, bool _clamp);

        void getPose(Matrix4* _pose, const AnimationClip& _clip, float _time, bool _clamp);

        uint32_t boneCount() const { return mBoneCount; }

        /**
         * \brief It is easier for the user to remember a name than an index. However using index is more efficient.
         * \note We assume that each bone in a skeleton has a different name. If there are bones with the same name, only the index of the last one is recorded.
         *
         * \return The index of the bone with specified name. Return uint32(-1) if not found.
         */
        uint32 getBoneIndex(const std::string& _name);

        const BoneInfo& getBoneInfo(uint32 _idx) const { return mBoneInfos[_idx]; }

        const Matrix4& getInvBindMatrix(uint32 _idx) const { return mInvBindMats[_idx]; }

        static std::shared_ptr<Skeleton> Create(ArrayProxy<BoneDesc> _bones);

    private:
        explicit Skeleton(ArrayProxy<BoneDesc> _bones);

        /**
         * \brief Sort the bone index using Topological Sorting so that a parent bone will always be calculated before its children when we calculate pose.
         * We can also avoid recursion operation.
         */
        void sortIdx();

        void buildMap();

        uint32_t mBoneCount = 0;
        TRS* mTransforms = nullptr;
        Matrix4* mInvBindMats = nullptr;
        BoneInfo* mBoneInfos = nullptr;

        uint32* mSortedIdx = nullptr;

        std::unordered_map<std::string, uint32> mNameIndexMap;
    };
}

#endif

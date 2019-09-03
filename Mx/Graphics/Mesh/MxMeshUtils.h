#pragma once
#ifndef MX_MESH_UTILS_H_
#define MX_MESH_UTILS_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "../../Math/MxVector3.h"
#include "MxMesh.h"

namespace Mix {
    class MeshUtils :GeneralBase::StaticBase {
    public:

        static std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> Sphere(float _radius, uint32_t _stacks, uint32_t _sectors);

        static std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> Box(const Math::Vector3f& _halfExtent);

        static std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> Quad(const Math::Vector2f& _halfExtent);

        static std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> Capsule(float _radius, float _halfLength, uint32_t _halfStacks, uint32_t _sections);
    };
}

#endif

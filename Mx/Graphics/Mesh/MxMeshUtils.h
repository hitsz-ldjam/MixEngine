#pragma once
#ifndef MX_MESH_UTILS_H_
#define MX_MESH_UTILS_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "../../Math/MxVector3.h"
#include "MxMesh.h"

namespace Mix {
    class MeshUtils :GeneralBase::StaticBase {
    public:

        static std::pair<std::vector<Vector3f>, std::vector<uint32_t>> Sphere(float _radius, uint32_t _stacks, uint32_t _sectors);

        static std::pair<std::vector<Vector3f>, std::vector<uint32_t>> Box(const Vector3f& _halfExtent);

        static std::pair<std::vector<Vector3f>, std::vector<uint32_t>> Quad(const Vector2f& _halfExtent);

        static std::pair<std::vector<Vector3f>, std::vector<uint32_t>> Capsule(float _radius, float _halfLength, uint32_t _halfStacks, uint32_t _sectors);

        static std::vector<Vector3f> CalculateNormals(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices);

        static void CalculateNormals(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, Vector3f* _normals);

        static std::pair<std::vector<Vector3f>, std::vector<Vector3f>> CalculateTangentsAndBitangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector3f> _normals, ArrayProxy<const Vector2f> _uvs);

        static void CalculateTangentsAndBitangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector3f> _normals, ArrayProxy<const Vector2f> _uvs,
                                                   Vector3f* _tangents, Vector3f* _bitangents);

        static std::vector<Vector3f> CalculateTangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector2f> _uvs);

        static void CalculateTangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector2f> _uvs,
                                      Vector3f* _tangents);
    };
}

#endif

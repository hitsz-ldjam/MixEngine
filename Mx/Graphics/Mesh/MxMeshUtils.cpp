#include "MxMeshUtils.h"

namespace Mix {
    class VertexConnectivity {
    public:

        VertexConnectivity(ArrayProxy<const uint32_t> _indices, size_t _vertexCount) {
            faceOfVertex.resize(_vertexCount);
            size_t faceCount = _indices.size() / 3;

            for (size_t i = 0; i < faceCount; ++i) {
                for (uint32_t j = 0; j < 3; ++j) {
                    size_t vIdx = _indices[i * 3 + j];
                    faceOfVertex[vIdx].push_back(i);
                }
            }
        }

        std::vector<std::vector<uint32_t>> faceOfVertex;

    };

    std::pair<std::vector<Vector3f>, std::vector<uint32_t>> MeshUtils::Sphere(float _radius, uint32_t _stacks, uint32_t _sectors) {
        assert(_radius > 0.0f && _stacks > 1 && _sectors > 2);

        size_t vertexCount = 2 + (_stacks - 1) * _sectors;
        size_t triangleCount = _sectors * (2 * _stacks - 2);
        size_t indexCount = triangleCount * 3;

        std::vector<Vector3f> vertices;
        vertices.reserve(vertexCount);

        float sectorStep = Math::Constants::Pi2 / _sectors;
        float stackStep = Math::Constants::Pi / _stacks;

        // The top vertex
        vertices.emplace_back(0.0f, _radius, 0.0f);

        for (uint32_t i = 1; i < _stacks; ++i) {
            float stackAngle = Math::Constants::Pi / 2 - i * stackStep;
            float xz = _radius * cosf(stackAngle);
            float y = _radius * sinf(stackAngle);

            for (uint32_t j = 0; j < _sectors; ++j) {
                float sectorAngle = j * sectorStep;
                // vertex position (x, y, z)
                vertices.emplace_back(xz * cosf(sectorAngle), y, xz * sinf(sectorAngle));
            }
        }

        // The bottom vertex
        vertices.emplace_back(0.0f, -_radius, 0.0f);

        std::vector<uint32_t> indices;
        indices.reserve(indexCount);

        // Triangles containing the top vertex
        for (uint32_t i = 0; i < _sectors; ++i) {
            indices.push_back(0);
            indices.push_back(1 + i);
            indices.push_back((2 + i) % _sectors);
        }

        for (uint32_t i = 0; i < _stacks - 1; ++i) {
            uint32_t k1 = i * _sectors + 1;
            uint32_t k2 = k1 + _sectors;

            for (uint32_t j = 0; j < _sectors; ++j, ++k1, ++k2) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        // Triangles containing the bottom vertex
        for (uint32_t i = 0; i < _sectors; ++i) {
            indices.push_back(vertexCount - 1);
            indices.push_back(vertexCount - 1 - _sectors + (2 + i) % _sectors);
            indices.push_back(vertexCount - 1 - _sectors + i);
        }

        return { vertices, indices };
    }

    std::pair<std::vector<Vector3f>, std::vector<uint32_t>> MeshUtils::Box(const Vector3f& _halfExtent) {
        assert(_halfExtent.x > 0.0f&&_halfExtent.y > 0.0f&&_halfExtent.z > 0.0f);

        std::vector<Vector3f> vertices;
        vertices.reserve(24);
        std::vector<uint32_t> indices;
        indices.reserve(36);

        float x = _halfExtent.x;
        float y = _halfExtent.y;
        float z = _halfExtent.z;

        vertices.emplace_back(x, -y, z);
        vertices.emplace_back(-x, -y, z);
        vertices.emplace_back(x, y, z);
        vertices.emplace_back(-x, y, z);
        vertices.emplace_back(-x, y, z);
        vertices.emplace_back(-x, -y, z);
        vertices.emplace_back(-x, y, -z);
        vertices.emplace_back(-x, -y, -z);
        vertices.emplace_back(x, y, z);
        vertices.emplace_back(-x, y, z);
        vertices.emplace_back(x, y, -z);
        vertices.emplace_back(-x, y, -z);
        vertices.emplace_back(-x, -y, z);
        vertices.emplace_back(x, -y, z);
        vertices.emplace_back(-x, -y, -z);
        vertices.emplace_back(x, -y, -z);
        vertices.emplace_back(x, -y, z);
        vertices.emplace_back(x, y, z);
        vertices.emplace_back(x, -y, -z);
        vertices.emplace_back(x, y, -z);
        vertices.emplace_back(x, -y, -z);
        vertices.emplace_back(x, y, -z);
        vertices.emplace_back(-x, -y, -z);
        vertices.emplace_back(-x, y, -z);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(6);
        indices.push_back(5);
        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(11);
        indices.push_back(10);
        indices.push_back(9);
        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(15);
        indices.push_back(14);
        indices.push_back(13);
        indices.push_back(16);
        indices.push_back(17);
        indices.push_back(18);
        indices.push_back(19);
        indices.push_back(18);
        indices.push_back(17);
        indices.push_back(20);
        indices.push_back(21);
        indices.push_back(22);
        indices.push_back(23);
        indices.push_back(22);
        indices.push_back(21);

        return { vertices, indices };
    }

    std::pair<std::vector<Vector3f>, std::vector<uint32_t>> MeshUtils::Quad(const Vector2f& _halfExtent) {
        std::vector<Vector3f> vertices;
        vertices.reserve(4);
        std::vector<uint32_t> indices;
        indices.reserve(6);

        vertices.emplace_back(_halfExtent.x, 0.0f, _halfExtent.y);
        vertices.emplace_back(-_halfExtent.x, 0.0f, _halfExtent.y);
        vertices.emplace_back(-_halfExtent.x, 0.0f, -_halfExtent.y);
        vertices.emplace_back(_halfExtent.x, 0.0f, -_halfExtent.y);

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(1);
        return { vertices, indices };
    }

    std::pair<std::vector<Vector3f>, std::vector<uint32_t>> MeshUtils::Capsule(float _radius, float _halfLength, uint32_t _halfStacks, uint32_t _sectors) {
        assert(_radius > 0.0f && _halfStacks > 0 && _sectors > 2);

        size_t vertexCount = 2 + _halfStacks * 2 * _sectors;
        size_t triangleCount = 4 * _halfStacks * _sectors;
        size_t indexCount = triangleCount * 3;

        std::vector<Vector3f> vertices;
        vertices.reserve(vertexCount);
        float sectorStep = Math::Constants::Pi2 / _sectors;
        float stackStep = 0.5f * Math::Constants::Pi / _halfStacks;

        // The top vertex
        vertices.emplace_back(0.0f, _radius + _halfLength, 0.0f);

        for (uint32_t i = 1; i < _halfLength; ++i) {
            float stackAngle = Math::Constants::Pi / 2 - i * stackStep;
            float xz = _radius * cosf(stackAngle);
            float y = _radius * sinf(stackAngle) + _halfLength;

            for (uint32_t j = 0; j < _sectors; ++j) {
                float sectorAngle = j * sectorStep;
                vertices.emplace_back(xz * cosf(sectorAngle), y, xz * sinf(sectorAngle));
            }
        }

        for (uint32_t i = 0; i < _halfLength - 1; ++i) {
            float stackAngle = Math::Constants::Pi / 2 - i * stackStep;
            float xz = _radius * cosf(stackAngle);
            float y = _radius * sinf(stackAngle) - _halfLength;

            for (uint32_t j = 0; j < _sectors; ++j) {
                float sectorAngle = j * sectorStep;
                vertices.emplace_back(xz * cosf(sectorAngle), y, xz * sinf(sectorAngle));
            }
        }

        // The bottom vertex
        vertices.emplace_back(0.0f, -_radius, 0.0f);

        std::vector<uint32_t> indices;
        indices.reserve(indexCount);

        // Triangles containing the top vertex
        for (uint32_t i = 0; i < _sectors; ++i) {
            indices.push_back(0);
            indices.push_back(1 + i);
            indices.push_back((2 + i) % _sectors);
        }

        for (uint32_t i = 1; i < _halfStacks - 1; ++i) {
            uint32_t k1 = i * (_sectors + 1);
            uint32_t k2 = k1 + _sectors + 1;

            for (uint32_t j = 0; j < _sectors; ++j, ++k1, ++k2) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        // Triangles containing the bottom vertex
        for (uint32_t i = 0; i < _sectors; ++i) {
            indices.push_back(vertexCount - 1);
            indices.push_back(vertexCount - 1 - _sectors + (2 + i) % _sectors);
            indices.push_back(vertexCount - 1 - _sectors + i);
        }

        return { vertices, indices };
    }

    std::vector<Vector3f> MeshUtils::CalculateNormals(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices) {
        std::vector<Vector3f> normals(_positions.size());
        CalculateNormals(_positions, _indices, normals.data());
        return normals;
    }

    void MeshUtils::CalculateNormals(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, Vector3f* _normals) {
        if (!_normals)
            return;

        auto vertexCount = _positions.size();
        auto faceCount = _indices.size() / 3;

        std::vector<Vector3f> faceNormals;
        faceNormals.reserve(faceCount);

        // Calculate normals for each face
        for (size_t i = 0; i < faceCount; ++i) {
            uint32_t triangle[3] = { _indices[i * 3 + 0], _indices[i * 3 + 1], _indices[i * 3 + 2] };

            Vector3f edgeA = _positions[triangle[1]] - _positions[triangle[0]];
            Vector3f edgeB = _positions[triangle[2]] - _positions[triangle[0]];
            faceNormals.push_back(edgeA.cross(edgeB).normalize());
        }

        VertexConnectivity connectivity(_indices, _positions.size());
        for (size_t i = 0; i < _positions.size(); ++i) {

            auto normal = Vector3f::Zero;
            for (auto faceId : connectivity.faceOfVertex[i]) {
                normal += faceNormals[faceId];
            }

            _normals[i] = normal.normalize();
        }
    }

    std::pair<std::vector<Vector3f>, std::vector<Vector3f>> MeshUtils::CalculateTangentsAndBitangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector3f> _normals, ArrayProxy<const Vector2f> _uvs) {
        std::vector<Vector3f> tangents(_positions.size());
        std::vector<Vector3f> bitangents(_positions.size());
        CalculateTangentsAndBitangents(_positions, _indices, _normals, _uvs, tangents.data(), bitangents.data());
        return { tangents, bitangents };
    }

    void MeshUtils::CalculateTangentsAndBitangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector3f> _normals, ArrayProxy<const Vector2f> _uvs,
                                                   Vector3f* _tangents, Vector3f* _bitangents) {
        size_t faceCount = _indices.size() / 3;
        size_t vertexCount = _positions.size();

        std::vector<Vector3f> faceTangents(faceCount);
        std::vector<Vector3f> faceBitangents(faceCount);
        for (size_t i = 0; i < faceCount; ++i) {
            uint32_t triangle[3] = { _indices[i * 3 + 0], _indices[i * 3 + 1], _indices[i * 3 + 2] };

            Vector3f p0 = _positions[triangle[0]];
            Vector3f p1 = _positions[triangle[1]];
            Vector3f p2 = _positions[triangle[2]];

            Vector2f uv0 = _uvs[triangle[0]];
            Vector2f uv1 = _uvs[triangle[1]];
            Vector2f uv2 = _uvs[triangle[2]];

            auto deltaPos1 = p1 - p0;
            auto deltaPos2 = p2 - p0;

            auto deltaUV1 = uv1 - uv0;
            auto deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            faceTangents[i] = (deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2) * r;
            faceBitangents[i] = (deltaUV1.x * deltaPos2 - deltaUV2.x * deltaPos1) * r;

            faceTangents[i] = faceTangents[i].normalize();
            faceBitangents[i] = faceBitangents[i].normalize();
        }

        VertexConnectivity connectivity(_indices, _positions.size());
        for (uint32_t i = 0; i < vertexCount; ++i) {
            auto& faces = connectivity.faceOfVertex[i];

            auto tangent = Vector3f::Zero;
            auto bitangent = Vector3f::Zero;

            for (uint32_t j = 0; j < faces.size(); ++j) {
                uint32_t faceIdx = faces[j];
                tangent += faceTangents[faceIdx];
                bitangent += faceBitangents[faceIdx];
            }

            tangent = tangent.normalize();
            bitangent = bitangent.normalize();

            // Orthonormalize
            auto& normal = _normals[i];
            float dot0 = normal.dot(tangent);
            _tangents[i] = (tangent - dot0 * normal).normalize();

            float dot1 = _tangents[i].dot(bitangent);
            dot0 = normal.dot(bitangent);
            _bitangents[i] = (bitangent - (dot0 * normal + dot1 * _tangents[i])).normalize();
        }
    }

    std::vector<Vector3f> MeshUtils::CalculateTangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector2f> _uvs) {
        std::vector<Vector3f> tangents(_positions.size());
        CalculateTangents(_positions, _indices, _uvs, tangents.data());
        return tangents;
    }

    void MeshUtils::CalculateTangents(ArrayProxy<const Vector3f> _positions, ArrayProxy<const uint32_t> _indices, ArrayProxy<const Vector2f> _uvs, Vector3f* _tangents) {
        size_t faceCount = _indices.size() / 3;
        size_t vertexCount = _positions.size();

        std::vector<Vector3f> faceTangents(faceCount);
        for (size_t i = 0; i < faceCount; ++i) {
            uint32_t triangle[3] = { _indices[i * 3 + 0], _indices[i * 3 + 1], _indices[i * 3 + 2] };

            Vector3f p0 = _positions[triangle[0]];
            Vector3f p1 = _positions[triangle[1]];
            Vector3f p2 = _positions[triangle[2]];

            Vector2f uv0 = _uvs[triangle[0]];
            Vector2f uv1 = _uvs[triangle[1]];
            Vector2f uv2 = _uvs[triangle[2]];

            auto deltaPos1 = p1 - p0;
            auto deltaPos2 = p2 - p0;

            auto deltaUV1 = uv1 - uv0;
            auto deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            faceTangents[i] = (deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2) * r;

            faceTangents[i] = faceTangents[i].normalize();
        }

        VertexConnectivity connectivity(_indices, _positions.size());
        for (uint32_t i = 0; i < vertexCount; ++i) {
            auto& faces = connectivity.faceOfVertex[i];

            auto tangent = Vector3f::Zero;
            auto bitangent = Vector3f::Zero;

            for (uint32_t j = 0; j < faces.size(); ++j) {
                tangent += faceTangents[faces[j]];
            }

            _tangents[i] = tangent.normalize();
        }
    }
}

#include "MxMeshUtils.h"

namespace Mix {

    std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> MeshUtils::Sphere(float _radius, uint32_t _stacks, uint32_t _sectors) {
        assert(_radius > 0.0f && _stacks > 1 && _sectors > 2);

        size_t vertexCount = 2 + (_stacks - 1) * _sectors;
        size_t triangleCount = _sectors * (2 * _stacks - 2);
        size_t indexCount = triangleCount * 3;

        std::vector<Math::Vector3f> vertices;
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

    std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> MeshUtils::Box(const Math::Vector3f& _halfExtent) {
        assert(_halfExtent.x > 0.0f&&_halfExtent.y > 0.0f&&_halfExtent.z > 0.0f);

        std::vector<Math::Vector3f> vertices;
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

    std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> MeshUtils::Quad(const Math::Vector2f& _halfExtent) {
        std::vector<Math::Vector3f> vertices;
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

    std::pair<std::vector<Math::Vector3f>, std::vector<uint32_t>> MeshUtils::Capsule(float _radius, float _halfLength, uint32_t _halfStacks, uint32_t _sectors) {
        assert(_radius > 0.0f && _halfStacks > 0 && _sectors > 2);

        size_t vertexCount = 2 + _halfStacks * 2 * _sectors;
        size_t triangleCount = 4 * _halfStacks * _sectors;
        size_t indexCount = triangleCount * 3;

        std::vector<Math::Vector3f> vertices;
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
}

#pragma once
#ifndef MX_MODEL_H_
#define MX_MODEL_H_

#include "../MxResource.h"
#include "../../MxVk/Buffers/MxVkBuffer.h"

namespace Mix {
    namespace Resource {
        class ResMesh :public ResourceBase {
        public:
            ResMesh(const Guid _guid, const std::string& _fileName, const std::filesystem::path& _filePath)
                : ResourceBase(_guid, _fileName, _filePath) {
            }

            struct SubmeshInfo {
                uint32_t firstVertex = 0;
                uint32_t vertexCount = 0;
                uint32_t firstIndex = 0;
                uint32_t indexCount = 0;

                SubmeshInfo() = default;

                SubmeshInfo(const uint32_t _firstVertex,
                            const uint32_t _vertexCount,
                            const uint32_t _firstIndex,
                            const uint32_t _indexCount) :
                    firstVertex(_firstVertex),
                    vertexCount(_vertexCount),
                    firstIndex(_firstIndex),
                    indexCount(_indexCount) {
                }
            };

            glm::mat4 transform = glm::mat4(1.0f);
            std::vector<SubmeshInfo> submeshes;

            // vulkan data
            Graphics::MemoryBlock vertexMem;
            std::shared_ptr<Graphics::Buffer> vertexBuffer;

            Graphics::MemoryBlock indexMem;
            std::shared_ptr<Graphics::Buffer> indexBuffer;
        };


        class ResModel :public ResourceBase {
        public:
            ResModel(const Guid _guid, const std::string& _fileName, const std::filesystem::path& _filePath)
                : ResourceBase(_guid, _fileName, _filePath) {
            }

            const vk::Buffer& getVertexBuffer() const { return vertexBuffer->buffer; }

            const uint32_t& getVertexCount()const { return vertexCount; }

            const uint32_t& getIndexCount()const { return indexCount; }

            const vk::Buffer& getIndexBuffer() const { return indexBuffer->buffer; }

            std::shared_ptr<Graphics::Buffer> vertexBuffer;
            uint32_t vertexCount = 0;

            std::shared_ptr<Graphics::Buffer> indexBuffer;
            uint32_t indexCount = 0;

            std::vector<ResourceRef> meshes;
        };
    }
}
#endif

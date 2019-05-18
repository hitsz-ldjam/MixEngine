#pragma once
#ifndef MX_MODEL_H_
#define MX_MODEL_H_

#include "../Resource/MxResource.h"
#include "../MxVk/MxVkAllocator.h"
#include "../MxVk/MxVkBuffer.h"

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

            std::vector<SubmeshInfo> submeshes;

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

            const vk::Buffer& GetVertexBuffer() const { return vertexBuffer->buffer; }

            const uint32_t& GetVertexCount()const { return vertexCount; }

            const uint32_t& GetIndexCount()const { return indexCount; }

            const vk::Buffer& GetIndexBuffer() const { return indexBuffer->buffer; }

            std::shared_ptr<Graphics::Buffer> vertexBuffer;
            uint32_t vertexCount = 0;

            std::shared_ptr<Graphics::Buffer> indexBuffer;
            uint32_t indexCount = 0;

            std::vector<ResourceRef> meshes;
        };
    }
}
#endif

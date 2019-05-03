#pragma once
#ifndef MX_MODEL_H_
#define MX_MODEL_H_

#include "../Resource/MxResource.h"

namespace Mix {
    namespace Resource {
        class Mesh :public ResourceBase {
            friend class ModelParseBase;
        public:


            Mesh(const Guid _guid, const std::string& _fileName, std::filesystem::path& _filePath)
                : ResourceBase(_guid, _fileName, _filePath) {
            }

            size_t SubmeshCount() const {
                return mSubmeshes.size();
            }

            struct SubmeshInfo {
                uint32_t firstVertex;
                uint32_t vertexCount;
                uint32_t firstIndex;
                uint32_t indexCount;
            };

            std::vector<SubmeshInfo> mSubmeshes;

        };


        class Model :public ResourceBase {
            friend class ModelParseBase;
        public:
            const vk::Buffer& GetVertexBuffer() const { return mVertexBuffer; }

            const uint32_t& GetVertexCount()const { return mVertexCount; }

            const uint32_t& GetIndexCount()const { return mIndexCount; }

            const vk::Buffer& GetIndexBuffer() const { return mIndexBuffer; }
        private:


            Graphics::MemoryBlock mVertexMem;
            vk::Buffer mVertexBuffer;
            uint32_t mVertexCount = 0;

            Graphics::MemoryBlock mIndexMem;
            vk::Buffer mIndexBuffer;
            uint32_t mIndexCount = 0;

            std::unordered_map<MeshId, ResourceRef> mMeshes;
        };
    }
}
#endif

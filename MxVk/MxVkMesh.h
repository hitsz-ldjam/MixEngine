#pragma once
#ifndef _MX_VK_MESH_H_
#define _MX_VK_MESH_H_


#include "MxVkDef.h"
#include "MxVkCore.h"
#include "MxVkAllocator.h"
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>


namespace Mix {
    namespace Graphics {
        //struct Vertex {
        //    glm::vec3 pos;
        //    glm::vec3 normal;
        //    glm::vec2 texCoord;

        //    //get vertex input description
        //    static std::vector<vk::VertexInputBindingDescription>& getBindingDescrip() {
        //        static std::vector<vk::VertexInputBindingDescription> bindingDescription = {
        //        vk::VertexInputBindingDescription(0,sizeof(Vertex),vk::VertexInputRate::eVertex) };

        //        return bindingDescription;
        //    }



        //    //get vertex input attributi description
        //    static std::vector<vk::VertexInputAttributeDescription>& getAttributeDescrip() {
        //        static std::vector<vk::VertexInputAttributeDescription> attributeDescription = {
        //            vk::VertexInputAttributeDescription(0,0,vk::Format::eR32G32B32Sfloat,offsetof(Vertex, pos)),
        //            vk::VertexInputAttributeDescription(1,0,vk::Format::eR32G32B32Sfloat,offsetof(Vertex, normal)),
        //            vk::VertexInputAttributeDescription(2,0,vk::Format::eR32G32Sfloat,offsetof(Vertex, texCoord))
        //        };

        //        return attributeDescription;
        //    }
        //};

        struct MeshInfo {
            vk::Buffer buffer;
            uint32_t firstVertex;
            uint32_t vertexCount;
            uint32_t triangleCount;
        };

        class MeshMgr :GraphicsComponent {
        private:
            typedef std::vector<float> MeshData;
            typedef std::vector<MeshData> ModelData;

            struct Info {
                MeshInfo meshInfo;
                MemoryBlock memoryBlock;

                Info() = default;
                Info(const MeshInfo& info, const MemoryBlock& block) :meshInfo(info), memoryBlock(block) {}
            };

        public:
            virtual ~MeshMgr() {
                destroy();
            }

            void init(std::shared_ptr<Core>& core, std::shared_ptr<DeviceAllocator>& allocator);

            void beginLoad(const vk::CommandBuffer& cmd);
            void loadModel(const std::string& name, fbxsdk::FbxScene * scene);
            void endLoad();

            const MeshInfo& getMesh(const std::string& name) {
                return mMeshInfos[name].meshInfo;
            }

            void destroy();

        private:
            bool mBegin = false;
            vk::DeviceSize mCurrSize = 0;
            vk::CommandBuffer mCmd;

            vk::DeviceSize mBufferSize = 8 * 1024 * 1024;
            Fence mFence;

            std::unordered_map<std::string, Info> mMeshInfos;

            std::shared_ptr<DeviceAllocator> mpAllocator;

        private:
            void processNode(fbxsdk::FbxNode* node, ModelData& modelData);

            void processMesh(fbxsdk::FbxMesh* node, ModelData& modelData);

            void readPosition(fbxsdk::FbxVector4* ctrPoints,
                              int ctrPointIndex,
                              MeshData& data) {
                data.push_back(static_cast<float>(ctrPoints[ctrPointIndex][0]));
                data.push_back(static_cast<float>(ctrPoints[ctrPointIndex][1]));
                data.push_back(static_cast<float>(ctrPoints[ctrPointIndex][2]));
            }

            void readUV(fbxsdk::FbxMesh* mesh,
                        fbxsdk::FbxGeometryElementUV* uv,
                        int ctrPointIndex,
                        int polygonIndex,
                        int vertexIndex,
                        MeshData& data);

            void readNormal(fbxsdk::FbxMesh* mesh,
                            fbxsdk::FbxGeometryElementNormal* normal,
                            int vertexCount,
                            MeshData& data);
        };
    }
}

// todo add glTF support
//#include <vector>
//
//#define TINYGLTF_IMPLEMENTATION
//#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define STB_IMAGE_IMPLEMENTATION
//#define STBI_MSC_SECURE_CRT
//#include <tiny_gltf.h>



#endif // !_MX_VK_MESH_H_

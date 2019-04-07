#pragma once
#ifndef _MX_UTILS_H_
#define _MX_UTILS_H_


#include <iostream>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

#include "../MxVk/MxVkDef.h"

#define STBI_MSC_SECURE_CRT
#include <glTF/tiny_gltf.h>

#define MAX_NUM_JOINTS 128u

//#include "fbxsdk.h"

namespace Mix {
    namespace Utils {
        // fbx is sb(so brilliant
        /*class FbxLoader {
        public:
            FbxLoader();
            ~FbxLoader();

            void LoadFBX(const std::string& name);

        private:
            fbxsdk::FbxManager* mManager;
            fbxsdk::FbxIOSettings* mIosettings = nullptr;
            fbxsdk::FbxImporter* mImporter = nullptr;
        };*/

        class GLTFLoader {
        public:
            struct TransformData {
                glm::vec3 translation = glm::vec3(0.0f);
                glm::quat rotation = glm::identity<glm::quat>();
                glm::vec3 scale = glm::vec3(1.0f);

                glm::mat4 matrix = glm::mat4(1.0f);
            };

            struct MeshData {
                std::string name;
                TransformData transform;
                size_t firstVertex;
                size_t vertexCount;
                size_t firstIndex;
                size_t indexCount;
            };

            struct ModelData {
                std::string name;
                std::vector<MeshData> meshes;
                std::vector<Graphics::Vertex> vertices;
                std::vector<uint32_t> indices;
            };

        public:
            ModelData loadFromGlb(const std::string& glb, const std::string& name);
            ModelData loadFromJson(const std::string& json, const std::string& name);
            void loadFromGLBStore(const std::string& glb, const std::string& name);
            void loadFromJsonStore(const std::string& json, const std::string& name);

            const ModelData& getModelData(const std::string& name);

            void clear() {
                mModels.clear();
            }

        private:
            tinygltf::TinyGLTF mLoader;

            std::unordered_map<std::string, ModelData> mModels;

            ModelData loadModel(const tinygltf::Model& model, const std::string& name);
            tinygltf::Model getGltfModelFromJson(const std::string& path);
            tinygltf::Model getGltfModelFromGlb(const std::string& path);

            void processNode(const tinygltf::Model& gltfModel,
                             TransformData& trans,
                             const tinygltf::Node * node,
                             ModelData& modelData);
        };

        class NextIdGenerator {
        public:
            NextIdGenerator() = default;
            NextIdGenerator(Id begin, IdStep step) :mNext(begin) {}

            Id next() {
                Id old = mNext;
                mNext += mStep;
                return old;
            }

            const Id& now() const {
                return mNext;
            }

        private:
            Id mNext = 0;
            IdStep mStep;
        };

        vk::DeviceSize align(vk::DeviceSize size, vk::DeviceSize alignment);
        vk::DeviceSize nextPowerOf2(vk::DeviceSize size);
        bool isPowerOf2(vk::DeviceSize size);
    }
}
#endif // !_MX_UTILS_H_

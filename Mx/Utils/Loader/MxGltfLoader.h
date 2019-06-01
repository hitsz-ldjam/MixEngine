#pragma once
#ifndef MX_UTILS_GLTFLOADER_H_
#define MX_UTILS_GLTFLOADER_H_

#include <string>
#include <vulkan/vulkan.hpp>
#include <chrono>

#include "../../Math/MxVector3.h"
#include "../../Math/MxQuaternion.h"
#include "../../Vulkan/Core/MxVkDef.h"
#include "../../Log/MxLog.h"

#define STBI_MSC_SECURE_CRT
#include <glTF/tiny_gltf.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Mix {
    namespace Utils {
        class GltfLoader {
        public:
            struct PrimitiveData {
                size_t firstVertex;
                size_t vertexCount;
                size_t firstIndex;
                size_t indexCount;

                int32_t material;
            };

            struct MeshData {
                std::string name;
				Math::Vector3f translation;
				Math::Quaternion rotation;
				Math::Vector3f scale;
                std::vector<PrimitiveData> primitives;
            };

            struct TextureSamplerData {
                vk::Filter minFilter = vk::Filter::eLinear;
                vk::Filter magFilter = vk::Filter::eLinear;

                struct {
                    vk::SamplerAddressMode U = vk::SamplerAddressMode::eClampToEdge;
                    vk::SamplerAddressMode V = vk::SamplerAddressMode::eClampToEdge;
                    vk::SamplerAddressMode W = vk::SamplerAddressMode::eClampToEdge;
                }addressMode;

                static vk::Filter ToVkFilter(int32_t _filterMode);
                static vk::SamplerAddressMode ToVkAddressMode(int32_t _addressMode);
            };

            struct ImageData {
                std::string name;
                uint32_t width = 0;
                uint32_t height = 0;
                std::vector<unsigned char> data;
            };

            struct TextureData {
                int32_t sampler;
                int32_t image;

                TextureData() = default;

                TextureData(const int32_t _sampler, const int32_t _image) :sampler(_sampler), image(_image) {}
            };

            struct MaterialData {
                enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
                AlphaMode alphaMode = ALPHAMODE_OPAQUE;

                float alphaCutoff = 1.0f;
                float metallicFactor = 1.0f;
                float roughnessFactor = 1.0f;

                Math::Vector4f baseColorFactor = Math::Vector4f::One;
				Math::Vector4f emissiveFactor = Math::Vector4f::One;

                int32_t baseColorTexture;
                int32_t metallicRoughnessTexture;
                int32_t normalTexture;
                int32_t occlusionTexture;
                int32_t emissiveTexture;

                struct TexCoordSets {
                    uint8_t baseColor = 0;
                    uint8_t metallicRoughness = 0;
                    uint8_t specularGlossiness = 0;
                    uint8_t normal = 0;
                    uint8_t occlusion = 0;
                    uint8_t emissive = 0;
                }texCoordSets;
            };

            struct ModelData {
                std::string name;
                std::vector<MeshData> meshes;
                std::vector<Graphics::Vertex> vertices;
                std::vector<uint32_t> indices;

                // texture
                std::vector<TextureSamplerData> textureSamplers;
                std::vector<TextureData> textures;
                std::vector<ImageData> images;
                std::vector<MaterialData> materials;
            };

            bool loadFromGlb(ModelData& _modelData, const std::filesystem::path& _glb, const std::string& _name);

            bool loadFromJson(ModelData& _modelData, const std::filesystem::path& _json, const std::string& _name);

        private:
            tinygltf::TinyGLTF mLoader;

            bool getGltfModelFromJson(tinygltf::Model& _model, const std::filesystem::path& _path);

            bool getGltfModelFromGlb(tinygltf::Model& _model, const std::filesystem::path& _path);

            ModelData loadModel(const tinygltf::Model& _model, const std::string& _name) const;

            static void LoadTextureSamplers(const tinygltf::Model& _model, ModelData& _modelData);

            static void LoadTextures(const tinygltf::Model& _model, ModelData& _modelData);

            static void LoadImages(const tinygltf::Model& _model, ModelData& _modelData);

            static void LoadMaterials(const tinygltf::Model& _model, ModelData& _modelData);

            void processNode(const tinygltf::Model& _gltfModel,
                             const glm::mat4&       _trans,
                             const tinygltf::Node * _node,
                             ModelData&             _modelData) const;
        };
    }
}

#endif

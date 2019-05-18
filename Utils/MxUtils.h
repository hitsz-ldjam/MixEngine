#pragma once
#ifndef _MX_UTILS_H_
#define _MX_UTILS_H_


#include <iostream>
#include <string>
#include <vulkan/vulkan.hpp>
#include <chrono>

#include "../MxVk/MxVkDef.h"
#include "../Engine/MxLog.h"

#define STBI_MSC_SECURE_CRT
#include <glTF/tiny_gltf.h>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define MAX_NUM_JOINTS 128u

//#include "fbxsdk.h"

#define TO_STRING(a) (#a)

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
                glm::mat4 transform;
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

                glm::vec4 baseColorFactor = glm::vec4(1.0f);
                glm::vec4 emissiveFactor = glm::vec4(1.0f);

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

            bool LoadFromGlb(ModelData& _modelData, const std::filesystem::path& _glb, const std::string& _name);
            bool LoadFromJson(ModelData& _modelData, const std::filesystem::path& _json, const std::string& _name);

        private:
            tinygltf::TinyGLTF mLoader;

            bool GetGltfModelFromJson(tinygltf::Model& _model, const std::filesystem::path& _path);
            bool GetGltfModelFromGlb(tinygltf::Model& _model, const std::filesystem::path& _path);

            ModelData LoadModel(const tinygltf::Model& _model, const std::string& _name) const;
            static void LoadTextureSamplers(const tinygltf::Model& _model, ModelData& _modelData);
            static void LoadTextures(const tinygltf::Model& _model, ModelData& _modelData);
            static void LoadImages(const tinygltf::Model& _model, ModelData& _modelData);
            static void LoadMaterials(const tinygltf::Model& _model, ModelData& _modelData);

            void ProcessNode(const tinygltf::Model& _gltfModel,
                             const glm::mat4&       _trans,
                             const tinygltf::Node * _node,
                             ModelData&             _modelData) const;
        };

        class NextIdGenerator {
        public:
            NextIdGenerator() = default;
            NextIdGenerator(const Id _begin, const IdStep _step) :mNext(_begin), mStep(_step) {}

            Id Next() {
                const auto old = mNext;
                mNext += mStep;
                return old;
            }

            const Id& Now() const {
                return mNext;
            }

        private:
            Id mNext = 0;
            IdStep mStep = 0;
        };

        template<typename _Pre, typename _Lst>
        _Pre Align(_Pre const& _size, _Lst const& _alignment) {
            return (_size + _alignment - 1) & ~(_alignment - 1);
        }

        vk::DeviceSize NextPowerOf2(vk::DeviceSize _size);

        bool IsPowerOf2(vk::DeviceSize _size);

        class GuidGenerator {
        public:
            GuidGenerator() = delete;
            static long long GetGuid();
            static long long GetGuid(const std::string& _str);
            static long long GetGuid(const std::filesystem::path& path) {
                return hash_value(path);
            }

        private:
            static std::hash<std::string> sStringHash;
            static std::hash<long long> sTimeHash;
        };

        template<typename _Ty = uint32_t>
        struct OffsetSize {
            using ValueType = _Ty;

            OffsetSize() = default;

            OffsetSize(ValueType const& _offset, ValueType const& _size) :offset(_offset), size(_size) {}

            bool operator==(OffsetSize const& _other) { return offset == _other.offset && size == _other.size; }

            bool operator!=(OffsetSize const& _other) { return !*this == _other; }

            ValueType offset = 0;
            ValueType size = 0;
        };
    }
}
#endif // !_MX_UTILS_H_

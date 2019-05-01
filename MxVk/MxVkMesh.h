#pragma once
#ifndef _MX_VK_MESH_H_
#define _MX_VK_MESH_H_

#include "../Utils/MxUtils.h"
#include "MxVkDef.h"
#include "MxVkCore.h"
#include "MxVkAllocator.h"
#include "MxVkCommand.h"
#include "MxVkImage.h"
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>

// #define GLTF_ENABLED

#ifdef GLTF_ENABLED

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <glTF/tiny_gltf.h>

#define MAX_NUM_JOINTS 128u

#endif // GLTF_ENABLED


namespace Mix {
    namespace Graphics {
#pragma region FBX
        namespace fbx {
            struct MeshInfo {
                std::string name;
                vk::Buffer buffer;
                uint32_t firstVertex;
                uint32_t vertexCount;
                uint32_t triangleCount;
            };

            struct ModelInfo {
                std::string name;
                vk::Buffer buffer;
                std::vector<MeshInfo> meshs;
            };

            class MeshMgr :GraphicsComponent {
            private:
                struct MeshData {
                    std::vector<float> vertices;
                    std::string name;
                    // todo add matrix
                };

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
                    if (mMeshInfos.count(name) == 0)
                        throw std::runtime_error("Error : No mesh named [ " + name + " ]");

                    return mMeshInfos[name].meshInfo;
                }

                const ModelInfo& getModel(const std::string& name) {
                    if (mModelInfos.count(name) == 0)
                        throw std::runtime_error("Error : No mesh named [ " + name + " ]");

                    return mModelInfos[name];
                }

                void destroy();

            private:
                bool mBegin = false;
                vk::DeviceSize mCurrSize = 0;
                vk::CommandBuffer mCmd;

                vk::DeviceSize mBufferSize = 8 * 1024 * 1024;
                Fence mFence;

                std::unordered_map<std::string, Info> mMeshInfos;
                std::unordered_map<std::string, ModelInfo> mModelInfos;

                std::shared_ptr<DeviceAllocator> mpAllocator;

            private:
                void processNode(fbxsdk::FbxNode* node, ModelData& modelData);

                void processMesh(fbxsdk::FbxMesh* node, ModelData& modelData);

                void readPosition(fbxsdk::FbxVector4* ctrPoints,
                                  int ctrPointIndex,
                                  MeshData& data) {
                    data.vertices.push_back(static_cast<float>(ctrPoints[ctrPointIndex][0]));
                    data.vertices.push_back(static_cast<float>(ctrPoints[ctrPointIndex][1]));
                    data.vertices.push_back(static_cast<float>(ctrPoints[ctrPointIndex][2]));
                }

                void readUV(fbxsdk::FbxMesh* mesh,
                            fbxsdk::FbxGeometryElementUV* uv,
                            int ctrPointIndex,
                            int polygonIndex,
                            int vertexIndex,
                            MeshData& data);

                void readNormal(fbxsdk::FbxMesh* mesh,
                                int ctrlPointIndex,
                                int vertexCount,
                                MeshData& data);
            };
        }

#pragma endregion

#pragma region GLTF_NO_ANIMATION

        namespace gltf {
            struct MeshData {
                std::string name;
                vk::Buffer vertexBuffer;
                vk::Buffer indexBuffer;
                uint32_t firstVertex;
                uint32_t vertexCount;
                uint32_t firstIndex;
                uint32_t indexCount;
            };

            class MeshMgr :GraphicsComponent {
            public:
                virtual ~MeshMgr() {
                    destroy();
                }

                void init(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator);

                void beginLoad(const vk::CommandBuffer& cmd) {
                    if (!mBegin) {
                        mBegin = true;
                        mCmd = cmd;
                    }
                }

                std::pair<ModelId,std::vector<MeshId>> loadModel(const Utils::GLTFLoader::ModelData& modelData);

                void endLoad() {
                    mCmd = nullptr;
                }

                std::shared_ptr<MeshData> getMesh(const ModelId & modelId, const MeshId & meshId);

                void destroy();

            private:
                bool mBegin = false;
                vk::CommandBuffer mCmd;

                Fence mFence;

                std::shared_ptr<DeviceAllocator> mpAllocator;

                struct MeshInfo {
                    std::string name;
                    uint32_t firstVertex;
                    uint32_t vertexCount;
                    uint32_t firstIndex;
                    uint32_t indexCount;
                };

                struct ModelInfo {
                    std::string name;
                    vk::Buffer vertexBuffer;
                    MemoryBlock vertexMem;
                    MemoryBlock indexMem;
                    vk::Buffer indexBuffer;
                    std::unordered_map<MeshId, MeshInfo> meshes;
                };

                std::unordered_map<ModelId, ModelInfo> mModels;

                Utils::NextIdGenerator mNextModelId;
                Utils::NextIdGenerator mNextMeshId;
            };
        }

#pragma endregion


// todo add glTF support
#ifdef GLTF_ENABLED

#pragma region GLTF_ANIMATION
        namespace gltf {
            struct Node;

            struct BoundingBox {
                glm::vec3 min;
                glm::vec3 max;
                bool valid = false;

                BoundingBox() = default;
                BoundingBox(const glm::vec3& min, const glm::vec3& max) :min(min), max(max) {}
                BoundingBox getAABB(const glm::mat4& m);
            };

            struct TextureSampler {
                vk::Filter minFilter;
                vk::Filter magFilter;
                struct {
                    vk::SamplerAddressMode U;
                    vk::SamplerAddressMode V;
                    vk::SamplerAddressMode W;
                }addressMode;
            };

            //implement this in MxVkImage
            struct Texture {
                std::shared_ptr<Core> core;
                std::shared_ptr<DeviceAllocator> allocator;
                std::shared_ptr<CommandMgr> cmdMgr;

                vk::Image image;
                vk::ImageLayout imageLayout;
                vk::DeviceMemory deviceMemory;
                //MemoryBlock deviceMemory;
                vk::ImageView view;
                uint32_t width, height;
                uint32_t mipLevels;
                uint32_t layerCount;
                vk::DescriptorImageInfo descriptor;
                vk::Sampler sampler;

                void updateDescriptor();

                void destroy();

                /*
                    Load a texture from a glTF image (stored as vector of chars loaded via stb_image)
                    Also generates the mip chain as glTF images are stored as jpg or png without any mips
                */
                void fromglTfImage(tinygltf::Image &gltfimage, TextureSampler textureSampler);
            };

            struct Material {
                enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
                AlphaMode alphaMode = ALPHAMODE_OPAQUE;

                float alphaCutoff = 1.0f;
                float metallicFactor = 1.0f;
                float roughnessFactor = 1.0f;

                glm::vec4 baseColorFactor = glm::vec4(1.0f);
                glm::vec4 emissiveFactor = glm::vec4(1.0f);

                Texture *baseColorTexture;
                Texture *metallicRoughnessTexture;
                Texture *normalTexture;
                Texture *occlusionTexture;
                Texture *emissiveTexture;

                struct TexCoordSets {
                    uint8_t baseColor = 0;
                    uint8_t metallicRoughness = 0;
                    uint8_t specularGlossiness = 0;
                    uint8_t normal = 0;
                    uint8_t occlusion = 0;
                    uint8_t emissive = 0;
                }texCoordSets;

                struct Extension {
                    Texture *specularGlossinessTexture;
                    Texture *diffuseTexture;
                    glm::vec4 diffuseFactor = glm::vec4(1.0f);
                    glm::vec3 specularFactor = glm::vec3(0.0f);
                } extension;

                struct PbrWorkflows {
                    bool metallicRoughness = true;
                    bool specularGlossiness = false;
                } pbrWorkflows;

                vk::DescriptorSet descriptorSet = nullptr;
            };

            struct Primitive {
                uint32_t firstIndex;
                uint32_t indexCount;
                uint32_t vertexCount;
                Material& material;
                bool hasIndices;

                BoundingBox bb;

                Primitive(uint32_t firstIndex,
                          uint32_t indexCount,
                          uint32_t vertexCount,
                          Material &material) :
                    firstIndex(firstIndex),
                    indexCount(indexCount),
                    vertexCount(vertexCount),
                    material(material) {
                    hasIndices = indexCount > 0;
                };

                void setBoundingBox(const glm::vec3& min, glm::vec3& max) {
                    bb.min = min;
                    bb.max = max;
                    bb.valid = true;
                }

            };

            struct Mesh {
                std::shared_ptr<Core> core;
                std::shared_ptr<DeviceAllocator> allocator;

                std::string name;

                std::vector<Primitive*> primitives;

                BoundingBox bb;
                BoundingBox aabb;

                vk::Buffer buffer;
                MemoryBlock memBlock;
                vk::DescriptorBufferInfo descriptor;

                struct UniformBlock {
                    glm::mat4 matrix;
                    glm::mat4 jointMatrix[MAX_NUM_JOINTS];
                    float jointCount = 0.0f;
                }uniformBlock;

                Mesh(std::shared_ptr<Core> core,
                     std::shared_ptr<DeviceAllocator> allocator,
                     const glm::mat4 matrix);

                ~Mesh() {
                    core->device().destroyBuffer(buffer);
                    allocator->deallocate(memBlock);
                    for (auto p : primitives)
                        delete p;
                }

                void setBoundingBox(const glm::vec3& min, const glm::vec3& max) {
                    bb.min = min;
                    bb.max = max;
                    bb.valid = true;
                }
            };

            struct Skin {
                std::string name;
                Node* skeletonRoot = nullptr;
                std::vector<glm::mat4> inverseBindMats;
                std::vector<Node*> joints;
            };

            struct Node {
                Node *parent;
                uint32_t index;
                std::vector<Node*> children;
                glm::mat4 matrix;
                std::string name;
                Mesh *mesh;
                Skin *skin;
                int32_t skinIndex = -1;
                glm::vec3 translation{};
                glm::vec3 scale = glm::vec3(1.0f);
                glm::quat rotation;
                BoundingBox bvh;
                BoundingBox aabb;

                glm::mat4 localMatrix() {
                    return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
                }

                glm::mat4 getMatrix() {
                    glm::mat4 m = localMatrix();
                    Node *p = parent;
                    while (p) {
                        m = p->localMatrix() * m;
                        p = p->parent;
                    }
                    return m;
                }

                void update();

                ~Node() {
                    if (mesh) {
                        delete mesh;
                    }
                    for (auto& child : children) {
                        delete child;
                    }
                }
            };

            struct AnimationChannel {
                enum PathType { TRANSLATION, ROTATION, SCALE };
                PathType path;
                Node* node;
                uint32_t samplerIndex;
            };

            struct AnimationSampler {
                enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
                InterpolationType interpolation;
                std::vector<float> inputs;
                std::vector<glm::vec4> outputsVec4;
            };

            struct Animation {
                std::string name;
                std::vector<AnimationSampler> samplers;
                std::vector<AnimationChannel> channels;
                float start = std::numeric_limits<float>::max();
                float end = std::numeric_limits<float>::min();
            };

            struct Model {
                std::shared_ptr<Core> core;
                std::shared_ptr<DeviceAllocator> allocator;
                std::shared_ptr<CommandMgr> cmdMgr;

                struct Vertex {
                    glm::vec3 pos;
                    glm::vec3 normal;
                    glm::vec2 uv0;
                    glm::vec2 uv1;
                    glm::vec4 joint0;
                    glm::vec4 weight0;
                };

                struct Vertices {
                    vk::Buffer buffer = nullptr;
                    MemoryBlock memory;
                }vertices;

                struct Indices {
                    int count;
                    vk::Buffer buffer = nullptr;
                    MemoryBlock memory;
                }indices;

                glm::mat4 aabb;

                std::vector<Node*> nodes;
                std::vector<Node*> linearNodes;

                std::vector<Skin*> skins;

                std::vector<Texture> textures;
                std::vector<TextureSampler> textureSamplers;
                std::vector<Material> materials;
                std::vector<Animation> animations;
                std::vector<std::string> extensions;

                struct Dimensions {
                    glm::vec3 min = glm::vec3(FLT_MIN);
                    glm::vec3 max = glm::vec3(-FLT_MAX);
                }dimensions;

                void destroy();

                Node* findNode(Node *parent, uint32_t index);

                Node* nodeFromIndex(uint32_t index);

                void loadNode(Node *parent, const tinygltf::Node &node,
                              uint32_t nodeIndex,
                              const tinygltf::Model &model,
                              std::vector<uint32_t>& indexBuffer,
                              std::vector<Vertex>& vertexBuffer,
                              float globalscale);

                void loadSkins(tinygltf::Model &gltfModel);

                void loadTextures(tinygltf::Model &gltfModel);

                vk::SamplerAddressMode getVkWrapMode(int32_t wrapMode);

                vk::Filter getVkFilterMode(int32_t filterMode);

                void loadTextureSamplers(tinygltf::Model &gltfModel);

                void loadMaterials(tinygltf::Model &gltfModel);

                void loadAnimations(tinygltf::Model &gltfModel);

                void loadFromFile(std::string filename, float scale = 1.0f);

                void calculateBoundingBox(Node *node, Node *parent);

                void getSceneDimensions();

                void drawNode(Node *node, vk::CommandBuffer cmdBuffer);

                void draw(vk::CommandBuffer cmdBuffer);

                void updateAnimation(uint32_t index, float time);
            };
        }

#pragma endregion
#endif // GLTF_ENABLED
    }
}

#endif // !_MX_VK_MESH_H_

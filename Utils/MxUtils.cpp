#include "MxUtils.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glTF/tiny_gltf.h>

namespace Mix {
    /*Mix::Utils::FbxLoader::FbxLoader() {
        mManager = fbxsdk::FbxManager::Create();
        mIosettings = fbxsdk::FbxIOSettings::Create(mManager, "iossettings");
        mImporter = fbxsdk::FbxImporter::Create(mManager, "importer");
    }

    void Utils::FbxLoader::LoadFBX(const std::string & name) {
    }*/
    namespace Utils {
        void GLTFLoader::loadFromGLBStore(const std::string & glb, const std::string & name) {
            if (mModels.count(name) != 0) {
                std::cerr << "GLTF Error : [ " + name + " ] has been loaded" << std::endl;
                return;
            }

            // load binary glTF from file
            tinygltf::Model gltfModel = getGltfModelFromGlb(glb);

            mModels[name] = loadModel(gltfModel, name);
        }

        GLTFLoader::ModelData GLTFLoader::loadFromGlb(const std::string & glb, const std::string & name) {
            return loadModel(getGltfModelFromGlb(glb), name);
        }

        GLTFLoader::ModelData GLTFLoader::loadFromJson(const std::string & json, const std::string & name) {
            return loadModel(getGltfModelFromJson(json), name);
        }

        void GLTFLoader::loadFromJsonStore(const std::string & json, const std::string & name) {
            if (mModels.count(name) != 0) {
                std::cerr << "GLTF Error : [ " + name + " ] has been loaded" << std::endl;
                return;
            }

            // load json glTF from file
            tinygltf::Model gltfModel = getGltfModelFromJson(json);

            mModels[name] = loadModel(gltfModel, name);
        }

        const GLTFLoader::ModelData & GLTFLoader::getModelData(const std::string & name) {
            if (mModels.count(name) == 0)
                throw std::runtime_error("GLTF Error : No Model named [ " + name + " ]");
            return mModels[name];
        }

        GLTFLoader::ModelData GLTFLoader::loadModel(const tinygltf::Model & gltfModel, const std::string & name) {
            ModelData model;
            model.name = name;
            TransformData trans;
            const tinygltf::Scene& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

            model.meshes.reserve(gltfModel.meshes.size());
            for (size_t i = 0; i < scene.nodes.size(); ++i) {
                const tinygltf::Node& node = gltfModel.nodes[scene.nodes[i]];
                processNode(gltfModel, trans, &node, model);
            }

            return std::move(model);
        }

        tinygltf::Model GLTFLoader::getGltfModelFromJson(const std::string & path) {
            tinygltf::Model gltfModel;
            std::string err;
            std::string warn;
            bool result = mLoader.LoadASCIIFromFile(&gltfModel, &err, &warn, path);

            if (!warn.empty())
                std::cerr << "GLTF Warning : " << warn << std::endl;

            if (!err.empty())
                std::cerr << "GLTF Error : " << err << std::endl;

            if (!result)
                std::cerr << "GLTF Failure : Failed to Load [ " << path << " ]" << std::endl;

            return std::move(gltfModel);
        }

        tinygltf::Model GLTFLoader::getGltfModelFromGlb(const std::string & path) {
            tinygltf::Model gltfModel;
            std::string err;
            std::string warn;
            bool result = mLoader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);

            if (!warn.empty())
                std::cerr << "GLTF Warning : " << warn << std::endl;

            if (!err.empty())
                std::cerr << "GLTF Error : " << err << std::endl;

            if (!result)
                std::cerr << "GLTF Failure : Failed to Load [ " << path << " ]" << std::endl;

            return std::move(gltfModel);
        }

        void GLTFLoader::processNode(const tinygltf::Model& gltfModel, TransformData& trans, const tinygltf::Node * node, ModelData& modelData) {
            // set transform
            TransformData localTrans;
            if (node->translation.size() == 3) {
                localTrans.translation = glm::make_vec3(node->scale.data());
            }

            if (node->rotation.size() == 4) {
                localTrans.rotation = glm::make_quat(node->rotation.data());
            }

            if (node->scale.size() == 3) {
                localTrans.scale = glm::make_vec3(node->scale.data());
            }

            if (node->matrix.size() == 16) {
                localTrans.matrix = glm::make_mat4x4(node->matrix.data());
            }

            trans.translation = trans.translation + localTrans.translation;
            trans.rotation = trans.rotation * localTrans.rotation;
            trans.scale = trans.scale*localTrans.scale;
            trans.matrix = trans.matrix*localTrans.matrix;

            if (node->mesh > -1) {
                MeshData meshData;
                const tinygltf::Mesh& mesh = gltfModel.meshes[node->mesh];

                meshData.name = mesh.name;
                meshData.transform = trans;

                const float *bufferPos;
                const float *bufferNormals;
                const float *bufferTexCoordSet0;

                meshData.firstVertex = modelData.vertices.size();
                meshData.firstIndex = modelData.indices.size();

                for (size_t i = 0; i < mesh.primitives.size(); ++i) {
                    bufferPos = nullptr;
                    bufferNormals = nullptr;
                    bufferTexCoordSet0 = nullptr;

                    const tinygltf::Primitive& primitive = mesh.primitives[i];
                    // vertices
                    {
                        assert(primitive.attributes.count("POSITION") != 0);

                        const tinygltf::Accessor& posAccessor = gltfModel.accessors[primitive.attributes.find("POSITION")->second];
                        const tinygltf::BufferView& posView = gltfModel.bufferViews[posAccessor.bufferView];
                        bufferPos = reinterpret_cast<const float *>(&(gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));


                        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                            const tinygltf::Accessor &normAccessor = gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
                            const tinygltf::BufferView &normView = gltfModel.bufferViews[normAccessor.bufferView];
                            bufferNormals = reinterpret_cast<const float *>(&(gltfModel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                        }

                        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                            const tinygltf::Accessor &uvAccessor = gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                            const tinygltf::BufferView &uvView = gltfModel.bufferViews[uvAccessor.bufferView];
                            bufferTexCoordSet0 = reinterpret_cast<const float *>(&(gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        }

                        modelData.vertices.reserve(modelData.vertices.size() + posAccessor.count);
                        for (size_t v = 0; v < posAccessor.count; ++v) {
                            Graphics::Vertex vertex;
                            vertex.pos = glm::make_vec3(&bufferPos[v * 3]);
                            vertex.normal = bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f);
                            vertex.uv = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec2(0.0f);

                            modelData.vertices.push_back(std::move(vertex));
                        }
                    }

                    // indices
                    if (primitive.indices > -1) {
                        const tinygltf::Accessor &accessor = gltfModel.accessors[primitive.indices > -1 ? primitive.indices : 0];
                        const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
                        const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

                        const void *dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        modelData.indices.reserve(modelData.indices.size() + accessor.count);

                        switch (accessor.componentType) {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        {
                            const uint32_t *buf = static_cast<const uint32_t*>(dataPtr);
                            modelData.indices.insert(modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        {
                            const uint16_t *buf = static_cast<const uint16_t*>(dataPtr);
                            modelData.indices.insert(modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        {
                            const uint8_t *buf = static_cast<const uint8_t*>(dataPtr);
                            modelData.indices.insert(modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        default:
                            std::cerr << "Index component type" << accessor.componentType << "not supported" << std::endl;
                        }
                    }
                }

                meshData.vertexCount = modelData.vertices.size() - meshData.firstVertex;
                meshData.indexCount = modelData.indices.size() - meshData.firstIndex;

                modelData.meshes.push_back(std::move(meshData));

                if (node->children.size() > 0) {
                    for (size_t i = 0; i < node->children.size(); ++i) {
                        const tinygltf::Node& child = gltfModel.nodes[node->children[i]];
                        processNode(gltfModel, trans, &child, modelData);
                    }
                }
            }
        }

        vk::DeviceSize align(vk::DeviceSize size, vk::DeviceSize alignment) {
            return (size + alignment - 1) & ~(alignment - 1);
        }

        vk::DeviceSize nextPowerOf2(vk::DeviceSize size) {
            vk::DeviceSize power = static_cast<vk::DeviceSize>(std::log2l(static_cast<long double>(size))) + 1;
            return static_cast<vk::DeviceSize>(1) << power;
        }

        bool isPowerOf2(vk::DeviceSize size) {
            vk::DeviceSize mask = 0;
            uint32_t power = static_cast<uint32_t>(std::log2l(static_cast<long double>(size)));

            for (uint32_t i = 0; i < power; ++i)
                mask += static_cast<vk::DeviceSize>(1) << i;

            return !(size & mask);
        }
    }
}



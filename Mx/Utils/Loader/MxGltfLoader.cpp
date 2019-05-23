#include "MxGltfLoader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <glTF/tiny_gltf.h>

namespace Mix {
    namespace Utils {
        bool GltfLoader::loadFromGlb(ModelData& _modelData, const std::filesystem::path& _glb, const std::string& _name) {
            tinygltf::Model model;
            const auto success = getGltfModelFromGlb(model, _glb);

            if (!success) {
                Debug::Log::Warning("%s: Failed to load [ %s ]", __FUNCTION__, _glb.string().c_str());
                return false;
            }

            _modelData = loadModel(model, _name);
            return true;
        }

        bool GltfLoader::loadFromJson(ModelData& _modelData, const std::filesystem::path& _json, const std::string& _name) {
            tinygltf::Model model;
            const auto success = getGltfModelFromJson(model, _json);

            if (!success) {
                Debug::Log::Warning("%s: Failed to load [ %s ]", __FUNCTION__, _json.string().c_str());
                return false;
            }

            _modelData = loadModel(model, _name);
            return true;
        }

        GltfLoader::ModelData GltfLoader::loadModel(const tinygltf::Model & _model, const std::string & _name) const {
            ModelData modelData;
            modelData.name = _name;

            LoadTextureSamplers(_model, modelData);
            LoadTextures(_model, modelData);
            LoadImages(_model, modelData);
            LoadMaterials(_model, modelData);

            glm::mat4 trans = glm::mat4(1.0f);
            const tinygltf::Scene& scene = _model.scenes[_model.defaultScene > -1 ? _model.defaultScene : 0];

            modelData.meshes.reserve(_model.meshes.size());
            for (auto i : scene.nodes) {
                const tinygltf::Node& node = _model.nodes[i];
                processNode(_model, trans, &node, modelData);
            }

            return modelData;
        }

        void GltfLoader::LoadTextureSamplers(const tinygltf::Model & _model, ModelData & _modelData) {
            _modelData.textureSamplers.reserve(_model.samplers.size() + 1);

            for (auto& sampler : _model.samplers) {
                TextureSamplerData samplerData;
                samplerData.minFilter = TextureSamplerData::ToVkFilter(sampler.minFilter);
                samplerData.magFilter = TextureSamplerData::ToVkFilter(sampler.magFilter);
                samplerData.addressMode.U = TextureSamplerData::ToVkAddressMode(sampler.wrapS);
                samplerData.addressMode.V = TextureSamplerData::ToVkAddressMode(sampler.wrapT);
                samplerData.addressMode.W = samplerData.addressMode.V;

                _modelData.textureSamplers.emplace_back(samplerData);
            }

            // default texture sampler
            _modelData.textureSamplers.emplace_back();
        }

        void GltfLoader::LoadTextures(const tinygltf::Model & _model, ModelData & _modelData) {
            _modelData.textures.reserve(_model.textures.size());
            for (auto &tex : _model.textures) {
                _modelData.textures.emplace_back(tex.sampler, tex.source);
            }
        }

        void GltfLoader::LoadImages(const tinygltf::Model & _model, ModelData & _modelData) {
            _modelData.images.reserve(_model.images.size());
            for (auto& image : _model.images) {
                ImageData imageData;
                imageData.name = image.name;
                imageData.width = image.width;
                imageData.height = image.height;
                imageData.data = image.image;

                _modelData.images.emplace_back(std::move(imageData));
            }
        }

        void GltfLoader::LoadMaterials(const tinygltf::Model & _model, ModelData & _modelData) {
            _modelData.materials.reserve(_model.materials.size());
            for (auto& material : _model.materials) {
                MaterialData materialData;

                // PBR workflow
                // baseColor
                {
                    auto it = material.values.find("baseColorFactor");
                    if (it != material.values.end()) {
                        materialData.baseColorFactor = glm::make_vec4<double>(it->second.ColorFactor().data());
                    }
                }

                {
                    auto it = material.values.find("baseColorTexture");
                    if (it != material.values.end()) {
                        materialData.baseColorTexture = it->second.TextureIndex();
                        materialData.texCoordSets.baseColor = it->second.TextureTexCoord();
                    }
                }// !baseColor

                // metallicFactor
                {
                    auto it = material.values.find("metallicFactor");
                    if (it != material.values.end()) {
                        materialData.metallicFactor = static_cast<float>(it->second.Factor());
                    }
                }// !metallicFactor

                // roughnessFactor
                {
                    auto it = material.values.find("roughnessFactor");
                    if (it != material.values.end()) {
                        materialData.roughnessFactor = static_cast<float>(it->second.Factor());
                    }
                }// !roughnessFactor

                // metallicRoughnessTexture
                {
                    // The metallic and roughness properties 
                    // are packed together in a single texture called metallicRoughnessTexture
                    auto it = material.values.find("metallicRoughnessTexture");
                    if (it != material.values.end()) {
                        materialData.metallicRoughnessTexture = it->second.TextureIndex();
                        materialData.texCoordSets.metallicRoughness = it->second.TextureTexCoord();
                    }
                }// !metallicRoughnessTexture
                // !PBR workflow

                // additionalMaps
                {
                    auto it = material.additionalValues.find("normalTexture");
                    if (it != material.additionalValues.end()) {
                        materialData.normalTexture = it->second.TextureIndex();
                        materialData.texCoordSets.normal = it->second.TextureTexCoord();
                    }
                }

                {
                    auto it = material.additionalValues.find("emissiveTexture");
                    if (it != material.additionalValues.end()) {
                        materialData.emissiveTexture = it->second.TextureIndex();
                        materialData.texCoordSets.emissive = it->second.TextureTexCoord();
                    }
                }

                {
                    auto it = material.additionalValues.find("occlusionTexture");
                    if (it != material.additionalValues.end()) {
                        materialData.occlusionTexture = it->second.TextureIndex();
                        materialData.texCoordSets.occlusion = it->second.TextureTexCoord();
                    }
                }// !addtionalMaps

                // emissiveFactor
                {
                    auto it = material.additionalValues.find("emissiveFactor");
                    if (it != material.additionalValues.end()) {
                        materialData.emissiveFactor = glm::vec4(glm::make_vec3<double>(it->second.ColorFactor().data()), 1.0);
                    }
                }// !emissiveFactor

                // alphaMode
                {
                    auto it = material.additionalValues.find("alphaMode");
                    if (it != material.additionalValues.end()) {
                        tinygltf::Parameter param = it->second;
                        if (param.string_value == "BLEND") {
                            materialData.alphaMode = MaterialData::ALPHAMODE_BLEND;
                        }
                        if (param.string_value == "MASK") {
                            materialData.alphaCutoff = 0.5f;
                            materialData.alphaMode = MaterialData::ALPHAMODE_MASK;
                        }
                    }
                }// !alphaMode

                // alphaCutoff
                {
                    auto it = material.additionalValues.find("alphaCutoff");
                    if (it != material.additionalValues.end()) {
                        materialData.alphaCutoff = static_cast<float>(it->second.Factor());
                    }
                }// !alphaCutoff

                // todo we don't support any extension now
                // Extensions
                /*if (material.extensions.find("KHR_materials_pbrSpecularGlossiness") != mat.extensions.end()) {
                    auto ext = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                    if (ext->second.Has("specularGlossinessTexture")) {
                        auto index = ext->second.Get("specularGlossinessTexture").Get("index");
                        material.extension.specularGlossinessTexture = &textures[index.Get<int>()];
                        auto texCoordSet = ext->second.Get("specularGlossinessTexture").Get("texCoord");
                        material.texCoordSets.specularGlossiness = texCoordSet.Get<int>();
                        material.pbrWorkflows.specularGlossiness = true;
                    }
                    if (ext->second.Has("diffuseTexture")) {
                        auto index = ext->second.Get("diffuseTexture").Get("index");
                        material.extension.diffuseTexture = &textures[index.Get<int>()];
                    }
                    if (ext->second.Has("diffuseFactor")) {
                        auto factor = ext->second.Get("diffuseFactor");
                        for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
                            auto val = factor.Get(i);
                            material.extension.diffuseFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
                        }
                    }
                    if (ext->second.Has("specularFactor")) {
                        auto factor = ext->second.Get("specularFactor");
                        for (uint32_t i = 0; i < factor.ArrayLen(); i++) {
                            auto val = factor.Get(i);
                            material.extension.specularFactor[i] = val.IsNumber() ? (float)val.Get<double>() : (float)val.Get<int>();
                        }
                    }
                }*/

                _modelData.materials.emplace_back(materialData);
            }
            // todo
            // Push a default material at the end of the list for mMeshes with no material assigned
        }

        bool GltfLoader::getGltfModelFromJson(tinygltf::Model& _model, const std::filesystem::path& _path) {
            std::string err;
            std::string warn;
            const bool success = mLoader.LoadASCIIFromFile(&_model, &err, &warn, _path.generic_string());

            if (!warn.empty())
                std::cerr << "Warning : " << __FUNCTION__": " << warn << std::endl;

            if (!err.empty())
                std::cerr << "Error : " << __FUNCTION__": " << err << std::endl;

            return success;
        }

        bool GltfLoader::getGltfModelFromGlb(tinygltf::Model& _model, const std::filesystem::path& _path) {
            std::string err;
            std::string warn;
            const bool success = mLoader.LoadBinaryFromFile(&_model, &err, &warn, _path.generic_string());

            if (!warn.empty())
                std::cerr << "Warning : " << __FUNCTION__": " << warn << std::endl;

            if (!err.empty())
                std::cerr << "Error : " << __FUNCTION__": " << err << std::endl;

            return success;
        }

        void GltfLoader::processNode(const tinygltf::Model& _gltfModel,
                                     const glm::mat4&       _trans,
                                     const tinygltf::Node * _node,
                                     ModelData&             _modelData) const {
            // set transform
            glm::mat4 localTrans = glm::mat4(1.0f);
            if (_node->matrix.size() == 16) {
                localTrans = glm::make_mat4(_node->matrix.data());
            } else {
                if (_node->scale.size() == 3) {
                    glm::vec3 scale = glm::make_vec3(_node->scale.data());
                    localTrans = glm::scale(localTrans, scale);
                }

                if (_node->rotation.size() == 4) {
                    localTrans = glm::toMat4(glm::make_quat(_node->rotation.data()));
                }

                if (_node->translation.size() == 3) {
                    glm::vec3 trans = glm::make_vec3(_node->translation.data());
                    localTrans = glm::translate(localTrans, trans);
                }
            }

            localTrans = _trans * localTrans;


            if (_node->mesh > -1) {
                MeshData meshData;
                const tinygltf::Mesh& mesh = _gltfModel.meshes[_node->mesh];

                meshData.name = mesh.name;
                meshData.transform = localTrans;


                const float *bufferPos;
                const float *bufferNormals;
                const float *bufferTexCoordSet0;

                meshData.primitives.reserve(mesh.primitives.size());
                for (const auto& primitive : mesh.primitives) {
                    PrimitiveData primitiveData{};

                    primitiveData.material = primitive.material;
                    primitiveData.firstVertex = _modelData.vertices.size();
                    primitiveData.firstIndex = _modelData.indices.size();

                    bufferPos = nullptr;
                    bufferNormals = nullptr;
                    bufferTexCoordSet0 = nullptr;

                    // vertices
                    {
                        assert(primitive.attributes.count("POSITION") != 0);

                        const tinygltf::Accessor& posAccessor = _gltfModel.accessors[primitive.attributes.find("POSITION")->second];
                        const tinygltf::BufferView& posView = _gltfModel.bufferViews[posAccessor.bufferView];
                        bufferPos = reinterpret_cast<const float *>(&(_gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));


                        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                            const tinygltf::Accessor &normAccessor = _gltfModel.accessors[primitive.attributes.find("NORMAL")->second];
                            const tinygltf::BufferView &normView = _gltfModel.bufferViews[normAccessor.bufferView];
                            bufferNormals = reinterpret_cast<const float *>(&(_gltfModel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                        }

                        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                            const tinygltf::Accessor &uvAccessor = _gltfModel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                            const tinygltf::BufferView &uvView = _gltfModel.bufferViews[uvAccessor.bufferView];
                            bufferTexCoordSet0 = reinterpret_cast<const float *>(&(_gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        }

                        _modelData.vertices.reserve(_modelData.vertices.size() + posAccessor.count);
                        for (size_t v = 0; v < posAccessor.count; ++v) {
                            Graphics::Vertex vertex{};
                            vertex.pos = glm::make_vec3(&bufferPos[v * 3]);
                            vertex.normal = bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f);
                            // F*** uv in gltf is x:[0,1],y:[1,2]
                            vertex.uv = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) - glm::vec2(0.0f, 1.0f) : glm::vec2(0.0f);

                            _modelData.vertices.push_back(vertex);
                        }
                    }

                    // indices
                    if (primitive.indices > -1) {
                        const tinygltf::Accessor &accessor = _gltfModel.accessors[primitive.indices > -1 ? primitive.indices : 0];
                        const tinygltf::BufferView &bufferView = _gltfModel.bufferViews[accessor.bufferView];
                        const tinygltf::Buffer &buffer = _gltfModel.buffers[bufferView.buffer];

                        const void *dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        _modelData.indices.reserve(_modelData.indices.size() + accessor.count);

                        switch (accessor.componentType) {
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                        {
                            const auto*buf = static_cast<const uint32_t*>(dataPtr);
                            _modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                        {
                            const auto*buf = static_cast<const uint16_t*>(dataPtr);
                            _modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                        {
                            const auto*buf = static_cast<const uint8_t*>(dataPtr);
                            _modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
                            break;
                        }
                        default:
                            std::cerr << "Index component type" << accessor.componentType << "not supported" << std::endl;
                        }
                    }

                    primitiveData.vertexCount = _modelData.vertices.size() - primitiveData.firstVertex;
                    primitiveData.indexCount = _modelData.indices.size() - primitiveData.firstIndex;

                    meshData.primitives.emplace_back(primitiveData);
                }

                _modelData.meshes.push_back(std::move(meshData));
            }

            if (!_node->children.empty()) {
                for (auto i : _node->children) {
                    processNode(_gltfModel, localTrans, &_gltfModel.nodes[i], _modelData);
                }
            }
        }

        vk::Filter GltfLoader::TextureSamplerData::ToVkFilter(const int32_t _filterMode) {
            switch (_filterMode) {
            case 9728:
                return vk::Filter::eNearest;
            case 9729:
                return vk::Filter::eLinear;
            case 9984:
                return vk::Filter::eNearest;
            case 9985:
                return vk::Filter::eNearest;
            case 9986:
                return vk::Filter::eLinear;
            case 9987:
                return vk::Filter::eLinear;
            default:
                std::cerr << "Unknown filter mode" << std::endl;
                return vk::Filter::eLinear;
            }
        }

        vk::SamplerAddressMode GltfLoader::TextureSamplerData::ToVkAddressMode(const int32_t _addressMode) {
            switch (_addressMode) {
            case 10497:
                return vk::SamplerAddressMode::eRepeat;
            case 33071:
                return vk::SamplerAddressMode::eClampToEdge;
            case 33648:
                return vk::SamplerAddressMode::eMirroredRepeat;
            default:
                std::cerr << "Unknown address mode" << std::endl;
                return vk::SamplerAddressMode::eClampToEdge;
            }
        }
    }
}

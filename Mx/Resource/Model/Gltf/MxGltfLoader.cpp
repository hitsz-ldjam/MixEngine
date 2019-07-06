#include "../../../Math/MxMatrix4.h"
#include "../../../Math/MxPtrMake.h"
#include "../../../Log/MxLog.h"

#include "MxGltfLoader.h"
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON
#include <nlohmann/json.hpp>
#include <glTF/tiny_gltf.h>

namespace Mix {
	namespace Resource {
		bool GltfLoader::loadFromGlb(ModelData::Model&  _modelData, const std::filesystem::path& _glb,
									 const std::string& _name) {
			tinygltf::Model model;
			const auto      success = getGltfModelFromGlb(model, _glb);

			if (!success) {
				Debug::Log::Warning("%s: Failed to load [ %s ]", __FUNCTION__, _glb.string().c_str());
				return false;
			}

			_modelData = LoadModel(model, _name);
			return true;
		}

		bool GltfLoader::loadFromJson(ModelData::Model&  _modelData, const std::filesystem::path& _json,
									  const std::string& _name) {
			tinygltf::Model model;
			const auto      success = getGltfModelFromJson(model, _json);

			if (!success) {
				Debug::Log::Warning("%s: Failed to load [ %s ]", __FUNCTION__, _json.string().c_str());
				return false;
			}

			_modelData = LoadModel(model, _name);
			return true;
		}

		ModelData::Model GltfLoader::LoadModel(const tinygltf::Model& _model, const std::string& _name) {
			ModelData::Model modelData;
			modelData.name = _name;

			LoadMeshes(_model, modelData);
			LoadTextureSamplers(_model, modelData);
			LoadTextures(_model, modelData);
			LoadImages(_model, modelData);
			LoadMaterials(_model, modelData);

			modelData.rootNode.children.emplace(); // root node will have at least one child
			const tinygltf::Scene& scene = _model.scenes[_model.defaultScene > -1 ? _model.defaultScene : 0];

			for (auto i : scene.nodes) {
				ProcessNode(_model, _model.nodes[i], modelData.rootNode, modelData);
			}

			return modelData;
		}

		void GltfLoader::LoadTextureSamplers(const tinygltf::Model& _model, ModelData::Model& _modelData) {
			_modelData.textureSamplers.reserve(_model.samplers.size() + 1);

			for (auto& sampler : _model.samplers) {

				ModelData::TextureSampler samplerData;
				samplerData.minFilter = ToVkFilter(sampler.minFilter);
				samplerData.magFilter = ToVkFilter(sampler.magFilter);
				samplerData.addressMode.u = ToVkAddressMode(sampler.wrapS);
				samplerData.addressMode.v = ToVkAddressMode(sampler.wrapT);
				samplerData.addressMode.w = samplerData.addressMode.v;

				_modelData.textureSamplers.emplace_back(samplerData);
			}

			// default texture sampler
			_modelData.textureSamplers.emplace_back();
		}

		void GltfLoader::LoadTextures(const tinygltf::Model& _model, ModelData::Model& _modelData) {
			_modelData.textures.reserve(_model.textures.size());
			for (auto& tex : _model.textures) {
				_modelData.textures.emplace_back(tex.sampler, tex.source);
			}
		}

		void GltfLoader::LoadImages(const tinygltf::Model& _model, ModelData::Model& _modelData) {
			_modelData.images.reserve(_model.images.size());
			for (auto& image : _model.images) {
				ModelData::Image imageData;
				imageData.name = image.name;
				imageData.width = image.width;
				imageData.height = image.height;
				imageData.data = image.image;

				_modelData.images.emplace_back(std::move(imageData));
			}
		}

		void GltfLoader::LoadMaterials(const tinygltf::Model& _model, ModelData::Model& _modelData) {
			_modelData.materials.reserve(_model.materials.size());
			for (auto& material : _model.materials) {
				ModelData::Material materialData;

				// PBR workflow
				// baseColor
				{
					auto it = material.values.find("baseColorFactor");
					if (it != material.values.end()) {
						materialData.baseColorFactor = Math::MakeVector4(it->second.ColorFactor().data());
					}
				}

				{
					auto it = material.values.find("baseColorTexture");
					if (it != material.values.end()) {
						materialData.baseColorTexture = it->second.TextureIndex();
						materialData.texCoordSets.baseColor = it->second.TextureTexCoord();
					}
				} // !baseColor

				// metallicFactor
				{
					auto it = material.values.find("metallicFactor");
					if (it != material.values.end()) {
						materialData.metallicFactor = static_cast<float>(it->second.Factor());
					}
				} // !metallicFactor

				// roughnessFactor
				{
					auto it = material.values.find("roughnessFactor");
					if (it != material.values.end()) {
						materialData.roughnessFactor = static_cast<float>(it->second.Factor());
					}
				} // !roughnessFactor

				// metallicRoughnessTexture
				{
					// The metallic and roughness properties 
					// are packed together in a single texture called metallicRoughnessTexture
					auto it = material.values.find("metallicRoughnessTexture");
					if (it != material.values.end()) {
						materialData.metallicRoughnessTexture = it->second.TextureIndex();
						materialData.texCoordSets.metallicRoughness = it->second.TextureTexCoord();
					}
				} // !metallicRoughnessTexture
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
				} // !addtionalMaps

				// emissiveFactor
				{
					auto it = material.additionalValues.find("emissiveFactor");
					if (it != material.additionalValues.end()) {
						materialData.emissiveFactor = Math::Vector4f(Math::MakeVector3(it->second.ColorFactor().data()),
																	 1.0f);
					}
				} // !emissiveFactor

				// alphaMode
				{
					auto it = material.additionalValues.find("alphaMode");
					if (it != material.additionalValues.end()) {
						tinygltf::Parameter param = it->second;
						if (param.string_value == "BLEND") {
							materialData.alphaMode = ModelData::Material::ALPHAMODE_BLEND;
						}
						if (param.string_value == "MASK") {
							materialData.alphaCutoff = 0.5f;
							materialData.alphaMode = ModelData::Material::ALPHAMODE_MASK;
						}
					}
				} // !alphaMode

				// alphaCutoff
				{
					auto it = material.additionalValues.find("alphaCutoff");
					if (it != material.additionalValues.end()) {
						materialData.alphaCutoff = static_cast<float>(it->second.Factor());
					}
				} // !alphaCutoff

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

		void GltfLoader::LoadMeshes(const tinygltf::Model& _gltfModel, ModelData::Model& _modelData) {
			_modelData.meshes.reserve(_gltfModel.meshes.size());
			for (auto& mesh : _gltfModel.meshes) {
				auto& meshData = _modelData.meshes.emplace_back();
				const float* bufferPos;
				const float* bufferNormals;
				const float* bufferTexCoordSet0;


				meshData.primitives.reserve(mesh.primitives.size());
				for (const auto& primitive : mesh.primitives) {
					auto& primitiveData = meshData.primitives.emplace_back();

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
						bufferPos = reinterpret_cast<const float *>(&_gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);


						if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
							const tinygltf::Accessor& normAccessor = _gltfModel.accessors[primitive
								.attributes.find("NORMAL")->
								second];
							const tinygltf::BufferView& normView = _gltfModel.bufferViews[normAccessor.bufferView];
							bufferNormals = reinterpret_cast<const float *>(&(_gltfModel.buffers[
								normView.buffer].data[
									normAccessor.byteOffset + normView.byteOffset]));
						}

						if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
							const tinygltf::Accessor& uvAccessor = _gltfModel.accessors[primitive
								.attributes.find("TEXCOORD_0")->
								second];
							const tinygltf::BufferView& uvView = _gltfModel.bufferViews[uvAccessor.bufferView];
							bufferTexCoordSet0 = reinterpret_cast<const float *>(&(_gltfModel.buffers[
								uvView.buffer].
																				 data[uvAccessor.byteOffset + uvView.byteOffset]));
						}

						_modelData.vertices.reserve(_modelData.vertices.size() + posAccessor.count);
						for (size_t v = 0; v < posAccessor.count; ++v) {
							Graphics::Vertex vertex{};

							vertex.pos = glm::make_vec3(&bufferPos[v * 3]);
							
							vertex.normal = bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f);

							// F*** uv in gltf is x:[0,1],y:[1,2]
							vertex.uv = bufferTexCoordSet0
								? glm::make_vec2(&bufferTexCoordSet0[v * 2]) - glm::vec2(0.0f, 1.0f)
								: glm::vec2(0.0f);

							// to left hand
							vertex.pos.x *= -1.0f;
							// to left hand
							vertex.normal.x *= 1.0f;
							// to left hand
							vertex.uv.y = 1.0f - vertex.uv.y;

							_modelData.vertices.push_back(vertex);
						}
					}

					// indices
					if (primitive.indices > -1) {
						const tinygltf::Accessor& accessor = _gltfModel.accessors[primitive.indices > -1? primitive.indices: 0];
						const tinygltf::BufferView& bufferView = _gltfModel.bufferViews[accessor.bufferView];
						const tinygltf::Buffer&     buffer = _gltfModel.buffers[bufferView.buffer];

						const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
						_modelData.indices.reserve(_modelData.indices.size() + accessor.count);

						switch (accessor.componentType) {
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
						{
							const auto* buf = static_cast<const uint32_t*>(dataPtr);
							_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
						{
							const auto* buf = static_cast<const uint16_t*>(dataPtr);
							_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
						{
							const auto* buf = static_cast<const uint8_t*>(dataPtr);
							_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
							break;
						}
						default:
							std::cerr << "Index component type" << accessor.componentType << "not supported" << std::
								endl;
						}
					}

					primitiveData.vertexCount = _modelData.vertices.size() - primitiveData.firstVertex;
					primitiveData.indexCount = _modelData.indices.size() - primitiveData.firstIndex;
				}
			}
		}

		void GltfLoader::ConvertToLeftHanded(Math::Vector3f& _translation, Math::Quaternion& _rotation) {
			_translation = Math::Vector3f(-_translation.x, _translation.y, _translation.z);
			_rotation = Math::Quaternion(-_rotation.w, -_rotation.x, _rotation.y, _rotation.z);
		}

		bool GltfLoader::getGltfModelFromJson(tinygltf::Model& _model, const std::filesystem::path& _path) {
			std::string err;
			std::string warn;
			const bool  success = mLoader.LoadASCIIFromFile(&_model, &err, &warn, _path.generic_string());

			if (!warn.empty())
				std::cerr << "Warning : " << __FUNCTION__": " << warn << std::endl;

			if (!err.empty())
				std::cerr << "Error : " << __FUNCTION__": " << err << std::endl;

			return success;
		}

		bool GltfLoader::getGltfModelFromGlb(tinygltf::Model& _model, const std::filesystem::path& _path) {
			std::string err;
			std::string warn;
			const bool  success = mLoader.LoadBinaryFromFile(&_model, &err, &warn, _path.generic_string());

			if (!warn.empty())
				std::cerr << "Warning : " << __FUNCTION__": " << warn << std::endl;

			if (!err.empty())
				std::cerr << "Error : " << __FUNCTION__": " << err << std::endl;

			return success;
		}

		void GltfLoader::ProcessNode(const tinygltf::Model& _gltfModel,
									 const tinygltf::Node&  _node,
									 ModelData::Node&       _parentNode,
									 ModelData::Model& _modelData) {
			// we didn't get camera info from gltf file
			// we just ignore the node without mesh or children
			if (!(_node.mesh > -1 || !_node.children.empty()))
				return;
			_parentNode.children.value().emplace_back();
			auto& node = _parentNode.children.value().back();
			node.name = _node.name;
			node.mesh = _node.mesh;

			// set transform
			if (_node.matrix.size() == 16) {
				Math::Matrix4::Decompose(Math::MakeMatrix4(_node.matrix.data()),
										 node.translation,
										 node.rotation,
										 node.scale);
			} else {
				if (_node.scale.size() == 3) {
					node.scale = Math::MakeVector3(_node.scale.data());
				}

				if (_node.rotation.size() == 4) {
					node.rotation = Math::Quaternion(_node.rotation[3],
													 _node.rotation[0],
													 _node.rotation[1],
													 _node.rotation[2]);
				}

				if (_node.translation.size() == 3) {
					node.translation = Math::MakeVector3(_node.translation.data());
				}
			}
			ConvertToLeftHanded(node.translation, node.rotation);

			if (!_node.children.empty()) {
				node.children.emplace(); // only if node has children
				for (auto i : _node.children) {
					ProcessNode(_gltfModel, _gltfModel.nodes[i], node, _modelData);
				}
			}
		}

		vk::Filter GltfLoader::ToVkFilter(const int32_t _filterMode) {
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
				Debug::Log::Error("Unknown filter mode: %d", _filterMode);
				return vk::Filter::eLinear;
			}
		}

		vk::SamplerAddressMode GltfLoader::ToVkAddressMode(const int32_t _addressMode) {
			switch (_addressMode) {
			case 10497:
				return vk::SamplerAddressMode::eRepeat;
			case 33071:
				return vk::SamplerAddressMode::eClampToEdge;
			case 33648:
				return vk::SamplerAddressMode::eMirroredRepeat;
			default:
				Debug::Log::Error("Unknown address mode: %d", _addressMode);
				return vk::SamplerAddressMode::eClampToEdge;
			}
		}
	}
}

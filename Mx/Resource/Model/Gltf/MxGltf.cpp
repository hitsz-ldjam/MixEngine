#include "MxGltf.h"
#include "../../../Log/MxLog.h"
#include "../../../Math/MxPtrMake.h"
#include "../../../Math/MxMatrix4.h"
#include "../../../Graphics/Texture/MxTexture.h"
#include <numeric>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#define TINYGLTF_NO_INCLUDE_JSON
#include <nlohmann/json.hpp>
#include <glTF/tiny_gltf.h>

namespace Mix {
	std::shared_ptr<ResourceBase> Gltf::load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) {
		std::shared_ptr<Model> model;
		if (_type == ResourceType::GLTF_BIN)
			model = loadFromGlb(_path);
		else if (_type == ResourceType::GLTF_ASCII)
			model = loadFromJson(_path);
		model->setName(_path.filename().generic_string());
		return model;
	}

	std::shared_ptr<ResourceBase> Gltf::load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) {
		if (_ext == "glb")
			return load(_path, ResourceType::GLTF_BIN, _additionalParam);
		if (_ext == "gltf")
			return load(_path, ResourceType::GLTF_ASCII, _additionalParam);

		return nullptr;
	}

	std::shared_ptr<Model> Gltf::loadFromGlb(const std::filesystem::path& _glb) {
		tinygltf::Model gltfModel;
		std::string err;
		std::string warn;
		const bool  success = mLoader.LoadBinaryFromFile(&gltfModel, &err, &warn, _glb.generic_string());

		if (!warn.empty())
			std::cerr << "Warning : " << warn << std::endl;

		if (!err.empty())
			std::cerr << "Error : " << err << std::endl;

		if (!success) {
			std::cerr << "Error : " << "Failed to load GLTF [" << _glb << "]" << std::endl;
			return nullptr;
		}

		return loadModel(gltfModel);
	}

	std::shared_ptr<Model> Gltf::loadFromJson(const std::filesystem::path& _glb) {
		tinygltf::Model gltfModel;
		std::string err;
		std::string warn;
		const bool  success = mLoader.LoadASCIIFromFile(&gltfModel, &err, &warn, _glb.generic_string());

		if (!warn.empty())
			std::cerr << "Warning : " << warn << std::endl;

		if (!err.empty())
			std::cerr << "Error : " << err << std::endl;

		if (!success) {
			std::cerr << "Error : " << "Failed to load GLTF [" << _glb << "]" << std::endl;
			return nullptr;
		}

		return loadModel(gltfModel);
	}

	std::shared_ptr<Model> Gltf::loadModel(const tinygltf::Model& _gltfModel) {
		mTempData.emplace();
		loadMeshes(_gltfModel);
		// loadTextures(_gltfModel);

		std::shared_ptr<Model> model = std::make_shared<Model>();

		const tinygltf::Scene& scene = _gltfModel.scenes[_gltfModel.defaultScene > -1 ? _gltfModel.defaultScene : 0];
		for (auto i : scene.nodes) {
			processNode(_gltfModel, _gltfModel.nodes[i], model->getRootNode());
		}

		model->setMeshes(std::move(mTempData->meshes));
		mTempData.reset();
		return model;
	}


	void Gltf::loadMeshes(const tinygltf::Model& _gltfModel) {
		mTempData->meshes.reserve(_gltfModel.meshes.size());
		for (auto& gltfMesh : _gltfModel.meshes) {
			size_t vertexCount = 0;
			{
				auto f = [&](size_t _sum, const tinygltf::Primitive& _p) {
					return _sum + _gltfModel.accessors[_p.attributes.find(GetGltfAttributeString(GltfAttribute::POSITION))->second].count;
				};

				vertexCount = std::accumulate(gltfMesh.primitives.begin(), gltfMesh.primitives.end(), size_t(0), f);
			}

			MixMeshData meshData;
			{
				auto& firstPrim = gltfMesh.primitives[0];
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::POSITION)) != firstPrim.attributes.end())
					meshData.positions.emplace(vertexCount);
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::NORMAL)) != firstPrim.attributes.end())
					meshData.normals.emplace(vertexCount);
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::TANGENT)) != firstPrim.attributes.end())
					meshData.tangents.emplace(vertexCount);
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::TEXCOORD_0)) != firstPrim.attributes.end())
					meshData.uv0.emplace(vertexCount);
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::TEXCOORD_1)) != firstPrim.attributes.end())
					meshData.uv1.emplace(vertexCount);
				if (firstPrim.attributes.find(GetGltfAttributeString(GltfAttribute::COLOR)) != firstPrim.attributes.end())
					meshData.colors.emplace(vertexCount);
				meshData.indices.emplace(gltfMesh.primitives.size());
				meshData.topologys.resize(gltfMesh.primitives.size());
				meshData.vertCount.resize(gltfMesh.primitives.size());
			}

			PopulateMeshAttributeData(meshData, _gltfModel, gltfMesh);
			mTempData->meshes.emplace_back(std::make_shared<Mesh>());
			ConstructMesh(*mTempData->meshes.back(), meshData);
		}
	}

	void Gltf::loadTextures(const tinygltf::Model& _gltfModel) {
		mTempData->textures.reserve(_gltfModel.textures.size());
		for (auto& gltfTex : _gltfModel.textures) {
			SamplerInfo samplerInfo;
			if (gltfTex.sampler != -1) {
				auto& gltfSampler = _gltfModel.samplers[gltfTex.sampler];
				samplerInfo.minFilter = GetTexFilterMode(gltfSampler.minFilter);
				samplerInfo.magFilter = GetTexFilterMode(gltfSampler.magFilter);
				samplerInfo.wrapModeU = GetTexWrapMode(gltfSampler.wrapS);
				samplerInfo.wrapModeV = GetTexWrapMode(gltfSampler.wrapT);
				samplerInfo.wrapModeW = samplerInfo.wrapModeU;
			}

			auto& gltfImage = _gltfModel.images[gltfTex.source];
			auto pixelCount = gltfImage.width * gltfImage.height;

			bool shouldDelete = false;
			const unsigned char* buffer;

			if (gltfImage.component == 3) {
				// We don't support RGB on Vulkan so convert to RGBA
				unsigned char* rgba = new unsigned char[pixelCount * 4];
				const unsigned char* rgb = gltfImage.image.data();

				for (int i = 0; i < gltfImage.width * gltfImage.height; ++i) {
					for (uint32_t j = 0; j < 3; ++j) {
						rgba[j] = rgb[j];
					}
					rgba += 4;
					rgb += 3;
				}

				buffer = rgba;
				shouldDelete = true;
			}
			else {
				buffer = gltfImage.image.data();
			}

			auto tex = std::make_shared<Texture2D>(gltfImage.width, gltfImage.height, TextureFormat::R8G8B8A8_Unorm);
			tex->setPixels(buffer, pixelCount * 4);
			tex->apply();
			mTempData->textures.push_back(tex);

			if (shouldDelete)
				delete[] buffer;
		}
	}

	void Gltf::processNode(const tinygltf::Model& _gltfModel, const tinygltf::Node& _gltfNode, Model::Node& _parentNode) const {
		Model::Node node(_gltfNode.name);
		node.setMeshRef(_gltfNode.mesh);

		Math::Vector3f translation;
		Math::Quaternion rotation;
		Math::Vector3f scale = Math::Vector3f::One;
		// set transform
		// If the transform is presented as a matrix
		if (_gltfNode.matrix.size() == 16) {
			Math::Matrix4::Decompose(Math::MakeMatrix4(_gltfNode.matrix.data()),
									 translation, rotation, scale);
		}
		else {
			if (_gltfNode.translation.size() == 3) {
				translation = Math::MakeVector3(_gltfNode.translation.data());
			}

			if (_gltfNode.rotation.size() == 4) {
				rotation = Math::Quaternion(_gltfNode.rotation[3], _gltfNode.rotation[0], _gltfNode.rotation[1], _gltfNode.rotation[2]);
			}

			if (_gltfNode.scale.size() == 3) {
				scale = Math::MakeVector3(_gltfNode.scale.data());
			}
		}
		ConvertTransformToLeftHanded(translation, rotation, scale);
		node.setTranslation(translation);
		node.setRotation(rotation);
		node.setScale(scale);

		if (!_gltfNode.children.empty()) {
			for (auto i : _gltfNode.children) {
				processNode(_gltfModel, _gltfModel.nodes[i], node);
			}
		}

		_parentNode.addChildNode(std::move(node));
	}

	std::pair<const std::byte*, size_t> Gltf::GetGltfAttributePtr(const tinygltf::Model& _gltfModel, const tinygltf::Primitive& _primitive,
																  GltfAttribute _attribute) {
		const tinygltf::Accessor& accessor = _gltfModel.accessors[_primitive.attributes.find(GetGltfAttributeString(_attribute))->second];
		const tinygltf::BufferView& bufferView = _gltfModel.bufferViews[accessor.bufferView];
		return { reinterpret_cast<const std::byte*>(&_gltfModel.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]),accessor.count };
	}

	void Gltf::PopulateMeshAttributeData(MixMeshData& _meshData, const tinygltf::Model& _gltfModel, const tinygltf::Mesh& _gltfMesh) {
		size_t vertOffset = 0;
		for (uint32_t i = 0; i < _gltfMesh.primitives.size(); ++i) {
			uint32_t vertCount = 0;
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::POSITION))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::POSITION);
				memcpy(_meshData.positions->data() + vertOffset, ptr, count * sizeof(MixMeshData::PositionType));
				vertCount = count;
			}
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::NORMAL))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::NORMAL);
				memcpy(_meshData.normals->data() + vertOffset, ptr, count * sizeof(MixMeshData::NormalType));
			}
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::TANGENT))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::TANGENT);
				memcpy(_meshData.tangents->data() + vertOffset, ptr, count * sizeof(MixMeshData::TangentType));
			}
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::TEXCOORD_0))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::TEXCOORD_0);
				memcpy(_meshData.uv0->data() + vertOffset, ptr, count * sizeof(MixMeshData::UVType));
			}
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::TEXCOORD_1))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::TEXCOORD_1);
				memcpy(_meshData.uv1->data() + vertOffset, ptr, count * sizeof(MixMeshData::UVType));
			}
			if (_gltfMesh.primitives[i].attributes.count(GetGltfAttributeString(GltfAttribute::COLOR))) {
				auto[ptr, count] = GetGltfAttributePtr(_gltfModel, _gltfMesh.primitives[i], GltfAttribute::COLOR);
				memcpy(_meshData.colors->data() + vertOffset, ptr, count * sizeof(MixMeshData::ColorType));
			}

			if (_gltfMesh.primitives[i].indices != -1) {
				const tinygltf::Accessor& accessor = _gltfModel.accessors[_gltfMesh.primitives[i].indices];
				const tinygltf::BufferView& bufferView = _gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = _gltfModel.buffers[bufferView.buffer];
				const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
				_meshData.indices.value()[i].resize(accessor.count);

				switch (accessor.componentType) {
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				{
					memcpy(_meshData.indices.value()[i].data(), dataPtr, sizeof(MixMeshData::IndexType)*accessor.count);
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				{
					const auto* buf = static_cast<const unsigned short*>(dataPtr);
					std::copy(buf, buf + accessor.count, _meshData.indices.value()[i].data());
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				{
					const auto* buf = static_cast<const unsigned char*>(dataPtr);
					std::copy(buf, buf + accessor.count, _meshData.indices.value()[i].data());
					break;
				}
				default:
					std::cerr << "Index component type" << accessor.componentType << "not supported" << std::endl;
				}
			}
			else {
				std::vector<MixMeshData::IndexType> indices(vertCount);
				for (size_t i = 0; i < vertCount; ++i) {
					indices[i] = i;
				}
				_meshData.indices.value()[i] = std::move(indices);
			}

			_meshData.topologys[i] = GetMeshTopology(_gltfMesh.primitives[i].mode);
			_meshData.vertCount[i] = vertCount;
			vertOffset += vertCount;
			//	bufferPos = nullptr;
			//	bufferNormals = nullptr;
			//	bufferTexCoordSet0 = nullptr;

			//	// vertices
			//	{
			//		assert(gltfPrimitive.attributes.count("POSITION") != 0);

			//		const tinygltf::Accessor& posAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("POSITION")->second];
			//		const tinygltf::BufferView& posView = _gltfModel.bufferViews[posAccessor.bufferView];
			//		bufferPos = reinterpret_cast<const float *>(&_gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);


			//		if (gltfPrimitive.attributes.find("NORMAL") != gltfPrimitive.attributes.end()) {
			//			const tinygltf::Accessor& normAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("NORMAL")->second];
			//			const tinygltf::BufferView& normView = _gltfModel.bufferViews[normAccessor.bufferView];
			//			bufferNormals = reinterpret_cast<const float *>(&(_gltfModel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
			//		}

			//		if (gltfPrimitive.attributes.find("TEXCOORD_0") != gltfPrimitive.attributes.end()) {
			//			const tinygltf::Accessor& uvAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("TEXCOORD_0")->second];
			//			const tinygltf::BufferView& uvView = _gltfModel.bufferViews[uvAccessor.bufferView];
			//			bufferTexCoordSet0 = reinterpret_cast<const float *>(&(_gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
			//		}

			//		_modelData.vertices.reserve(_modelData.vertices.size() + posAccessor.count);
			//		for (size_t v = 0; v < posAccessor.count; ++v) {
			//			Vulkan::Vertex vertex{};

			//			vertex.pos = glm::make_vec3(&bufferPos[v * 3]);

			//			vertex.normal = bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f);

			//			// F*** uv in gltf is x:[0,1],y:[1,2]
			//			vertex.uv = bufferTexCoordSet0
			//				? glm::make_vec2(&bufferTexCoordSet0[v * 2]) - glm::vec2(0.0f, 1.0f)
			//				: glm::vec2(0.0f);

			//			// to left hand
			//			vertex.pos.x *= -1.0f;
			//			// to left hand
			//			vertex.normal.x *= 1.0f;
			//			// to left hand
			//			vertex.uv.y = 1.0f - vertex.uv.y;

			//			_modelData.vertices.push_back(vertex);
			//		}
			//	}

			//	// indices
			//	if (gltfPrimitive.indices > -1) {
			//		const tinygltf::Accessor& accessor = _gltfModel.accessors[gltfPrimitive.indices > -1 ? gltfPrimitive.indices : 0];
			//		const tinygltf::BufferView& bufferView = _gltfModel.bufferViews[accessor.bufferView];
			//		const tinygltf::Buffer&     buffer = _gltfModel.buffers[bufferView.buffer];

			//		const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
			//		_modelData.indices.reserve(_modelData.indices.size() + accessor.count);

			//		switch (accessor.componentType) {
			//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
			//		{
			//			const auto* buf = static_cast<const uint32_t*>(dataPtr);
			//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
			//			break;
			//		}
			//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
			//		{
			//			const auto* buf = static_cast<const uint16_t*>(dataPtr);
			//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
			//			break;
			//		}
			//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
			//		{
			//			const auto* buf = static_cast<const uint8_t*>(dataPtr);
			//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
			//			break;
			//		}
			//		default:
			//			std::cerr << "Index component type" << accessor.componentType << "not supported" << std::
			//				endl;
			//		}
			//	}

			//	subMesh.vertexCount = _modelData.vertices.size() - subMesh.firstVertex;
			//	subMesh.indexCount = _modelData.indices.size() - subMesh.firstIndex;
			//}
		}
	}

	void Gltf::ConstructMesh(Mesh& _mesh, MixMeshData& _meshData) {
		// to left hand
		if (_meshData.positions.has_value()) {
			for (auto& vertex : _meshData.positions.value())
				vertex.x *= -1.0f;
		}
		if (_meshData.normals.has_value()) {
			for (auto& normal : _meshData.normals.value())
				normal.x *= -1.0f;
		}
		if (_meshData.uv0.has_value()) {
			for (auto& uv : _meshData.uv0.value())
				uv.y = 1.0f - uv.y;
		}
		if (_meshData.uv1.has_value()) {
			for (auto& uv : _meshData.uv1.value())
				uv.y = 1.0f - uv.y;
		}

		_mesh.setPositions(std::move(_meshData.positions).value_or(std::vector<MixMeshData::PositionType>()));
		_mesh.setNormals(std::move(_meshData.normals).value_or(std::vector<MixMeshData::NormalType>()));
		_mesh.setTangents(std::move(_meshData.tangents).value_or(std::vector<MixMeshData::TangentType>()));
		_mesh.setUVs(UVChannel::UV0, std::move(_meshData.uv0).value_or(std::vector<MixMeshData::UVType>()));
		_mesh.setUVs(UVChannel::UV1, std::move(_meshData.uv1).value_or(std::vector<MixMeshData::UVType>()));
		_mesh.setColors(std::move(_meshData.colors).value_or(std::vector<MixMeshData::ColorType>()));

		auto subMeshCount = _meshData.indices.value().size();
		uint32_t baseVertex = 0;
		for (uint32_t i = 0; i < subMeshCount; ++i) {
			_mesh.setIndices(std::move(_meshData.indices.value()[i]), _meshData.topologys[i], i, baseVertex);
			baseVertex += _meshData.vertCount[i];
		}
		_mesh.uploadMeshData(false);
	//for (const auto& gltfPrimitive : gltfMesh.primitives) {
		//	bufferPos = nullptr;
		//	bufferNormals = nullptr;
		//	bufferTexCoordSet0 = nullptr;

		//	// vertices
		//	{
		//		assert(gltfPrimitive.attributes.count("POSITION") != 0);

		//		const tinygltf::Accessor& posAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("POSITION")->second];
		//		const tinygltf::BufferView& posView = _gltfModel.bufferViews[posAccessor.bufferView];
		//		bufferPos = reinterpret_cast<const float *>(&_gltfModel.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);


		//		if (gltfPrimitive.attributes.find("NORMAL") != gltfPrimitive.attributes.end()) {
		//			const tinygltf::Accessor& normAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("NORMAL")->second];
		//			const tinygltf::BufferView& normView = _gltfModel.bufferViews[normAccessor.bufferView];
		//			bufferNormals = reinterpret_cast<const float *>(&(_gltfModel.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
		//		}

		//		if (gltfPrimitive.attributes.find("TEXCOORD_0") != gltfPrimitive.attributes.end()) {
		//			const tinygltf::Accessor& uvAccessor = _gltfModel.accessors[gltfPrimitive.attributes.find("TEXCOORD_0")->second];
		//			const tinygltf::BufferView& uvView = _gltfModel.bufferViews[uvAccessor.bufferView];
		//			bufferTexCoordSet0 = reinterpret_cast<const float *>(&(_gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
		//		}

		//		_modelData.vertices.reserve(_modelData.vertices.size() + posAccessor.count);
		//		for (size_t v = 0; v < posAccessor.count; ++v) {
		//			Vulkan::Vertex vertex{};

		//			vertex.pos = glm::make_vec3(&bufferPos[v * 3]);

		//			vertex.normal = bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f);

		//			// F*** uv in gltf is x:[0,1],y:[1,2]
		//			vertex.uv = bufferTexCoordSet0
		//				? glm::make_vec2(&bufferTexCoordSet0[v * 2]) - glm::vec2(0.0f, 1.0f)
		//				: glm::vec2(0.0f);

		//			// to left hand
		//			vertex.pos.x *= -1.0f;
		//			// to left hand
		//			vertex.normal.x *= 1.0f;
		//			// to left hand
		//			vertex.uv.y = 1.0f - vertex.uv.y;

		//			_modelData.vertices.push_back(vertex);
		//		}
		//	}

		//	// indices
		//	if (gltfPrimitive.indices > -1) {
		//		const tinygltf::Accessor& accessor = _gltfModel.accessors[gltfPrimitive.indices > -1 ? gltfPrimitive.indices : 0];
		//		const tinygltf::BufferView& bufferView = _gltfModel.bufferViews[accessor.bufferView];
		//		const tinygltf::Buffer&     buffer = _gltfModel.buffers[bufferView.buffer];

		//		const void* dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);
		//		_modelData.indices.reserve(_modelData.indices.size() + accessor.count);

		//		switch (accessor.componentType) {
		//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
		//		{
		//			const auto* buf = static_cast<const uint32_t*>(dataPtr);
		//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
		//			break;
		//		}
		//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
		//		{
		//			const auto* buf = static_cast<const uint16_t*>(dataPtr);
		//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
		//			break;
		//		}
		//		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
		//		{
		//			const auto* buf = static_cast<const uint8_t*>(dataPtr);
		//			_modelData.indices.insert(_modelData.indices.end(), buf, buf + accessor.count);
		//			break;
		//		}
		//		default:
		//			std::cerr << "Index component type" << accessor.componentType << "not supported" << std::
		//				endl;
		//		}
		//	}

		//	subMesh.vertexCount = _modelData.vertices.size() - subMesh.firstVertex;
		//	subMesh.indexCount = _modelData.indices.size() - subMesh.firstIndex;
		//}
	}

	void Gltf::ConvertTransformToLeftHanded(Math::Vector3f& _translation, Math::Quaternion& _rotation, Math::Vector3f& _scale) {
		_translation = Math::Vector3f(-_translation.x, _translation.y, _translation.z);
		_rotation = Math::Quaternion(-_rotation.w, -_rotation.x, _rotation.y, _rotation.z);
	}

	const std::string& Gltf::GetGltfAttributeString(GltfAttribute _gltfAttribute) {
		static const std::string strings[] = {
			"POSITION",
			"NORMAL",
			"TANGENT",
			"TEXCOORD_0",
			"TEXCOORD_1",
			"COLOR"
		};

		return strings[static_cast<uint32_t>(_gltfAttribute)];
	}

	VertexAttribute Gltf::GetVertexAttribute(GltfAttribute _gltfAttribute) {
		switch (_gltfAttribute) {
		case GltfAttribute::POSITION:	return VertexAttribute::Position;
		case GltfAttribute::NORMAL:		return VertexAttribute::Normal;
		case GltfAttribute::TANGENT:	return VertexAttribute::Tangent;
		case GltfAttribute::TEXCOORD_0: return VertexAttribute::UV0;
		case GltfAttribute::TEXCOORD_1: return VertexAttribute::UV1;
		case GltfAttribute::COLOR:		return VertexAttribute::Color;
		default:assert(false);
		}
	}

	MeshTopology Gltf::GetMeshTopology(int _gltfMode) {
		switch (_gltfMode) {
		case TINYGLTF_MODE_POINTS: return MeshTopology::Points_List;
		case TINYGLTF_MODE_LINE: return MeshTopology::Lines_List;
		case TINYGLTF_MODE_TRIANGLES: return MeshTopology::Triangles_List;
		default: return MeshTopology::Triangles_List;
		}
	}

	TextureFilterMode Gltf::GetTexFilterMode(int _gltfFilter) {
		switch (_gltfFilter) {
		case 9728:
		case 9984:
		case 9985:
			return TextureFilterMode::Nearest;
		case 9729:
		case 9986:
		case 9987:
			return TextureFilterMode::Linear;
		default:
			std::cerr << "Unknown filter mode: " << _gltfFilter << std::endl;
			return TextureFilterMode::Nearest;
		}
	}

	TextureWrapMode Gltf::GetTexWrapMode(int _gltfWrap) {
		switch (_gltfWrap) {
		case 10497:
			return TextureWrapMode::Repeat;
		case 33071:
			return TextureWrapMode::ClampToEdge;
		case 33648:
			return TextureWrapMode::Mirror;
		default:
			std::cerr << "Unknown wrap mode: " << _gltfWrap << std::endl;
			return TextureWrapMode::Repeat;
		}
	}
}

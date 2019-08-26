#pragma once
//#ifndef MX_GLTF_PARSER_H_
//#define MX_GLTF_PARSER_H_

#include "../MxModelParserBase.hpp"
#include "../../Texture/MxTexture.h"

#define STBI_MSC_SECURE_CRT
#define TINYGLTF_NO_INCLUDE_JSON
#include <nlohmann/json.hpp>
#include <glTF/tiny_gltf.h>

namespace Mix {
	class Texture;

	class Gltf final : public ModelParserBase {
	public:
		Gltf() {
			mSupportedTypes.insert(ResourceType::GLTF_BIN);
			mSupportedTypes.insert(ResourceType::GLTF_ASCII);

			mSupportedExts.insert("gltf");
			mSupportedExts.insert("glb");
		}

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) override;

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) override;
	private:
		enum class GltfAttribute {
			POSITION,
			NORMAL,
			TANGENT,
			TEXCOORD_0,
			TEXCOORD_1,
			COLOR
		};

		struct MixMeshData {
			using PositionType = Mesh::PositionType;
			using NormalType = Mesh::NormalType;
			using TangentType = Mesh::TangentType;
			using UVType = Mesh::UV2DType;
			using ColorType = Mesh::ColorType;
			using IndexType = Mesh::Index32Type;

			std::optional<std::vector<PositionType>> positions;
			std::optional<std::vector<NormalType>> normals;
			std::optional<std::vector<TangentType>> tangents;
			std::optional<std::vector<UVType>> uv0;
			std::optional<std::vector<UVType>> uv1;
			std::optional<std::vector<ColorType>> colors;
			std::vector<MeshTopology> topologys;
			std::optional<std::vector<std::vector<IndexType>>> indices;
			std::vector<uint32_t> vertCount;
		};

		static const std::string& GetGltfAttributeString(GltfAttribute _gltfAttribute);
		static VertexAttribute GetVertexAttribute(GltfAttribute _gltfAttribute);
		static MeshTopology GetMeshTopology(int _gltfMode);
		static TextureFilterMode GetTexFilterMode(int _gltfFilter);
		static TextureWrapMode GetTexWrapMode(int _gltfWrap);

		std::shared_ptr<Model> loadFromGlb(const std::filesystem::path& _glb);
		std::shared_ptr<Model> loadFromJson(const std::filesystem::path& _glb);

		std::shared_ptr<Model> loadModel(const tinygltf::Model& _gltfModel);
		void loadMeshes(const tinygltf::Model& _gltfModel);
		void loadTextures(const tinygltf::Model& _gltfModel);

		void processNode(const tinygltf::Model& _gltfModel, const tinygltf::Node&  _gltfNode, Model::Node& _parentNode) const;

		static  std::pair<const std::byte*, size_t> GetGltfAttributePtr(const tinygltf::Model& _gltfModel, const tinygltf::Primitive& _primitive, GltfAttribute _attribute);
		static void PopulateMeshAttributeData(MixMeshData& _meshData, const tinygltf::Model& _gltfModel, const tinygltf::Mesh& _gltfMesh);
		static void ConstructMesh(Mesh& _mesh, MixMeshData& _meshData);

		static void ConvertTransformToLeftHanded(Math::Vector3f& _translation, Math::Quaternion& _rotation, Math::Vector3f& _scale);

		struct TempData {
			std::vector<std::shared_ptr<Mesh>> meshes;
			std::vector<std::shared_ptr<Texture>> textures;
		};
		std::optional<TempData> mTempData;

		tinygltf::TinyGLTF mLoader;
	};
}


//#endif // !MX_GLTF_PARSER_H_

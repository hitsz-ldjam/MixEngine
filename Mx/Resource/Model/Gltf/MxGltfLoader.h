#pragma once
#ifndef MX_UTILS_GLTFLOADER_H_
#define MX_UTILS_GLTFLOADER_H_

#include <string>
#include <vulkan/vulkan.hpp>
#include "../MxModelData.h"

#define STBI_MSC_SECURE_CRT
#include <glTF/tiny_gltf.h>
#include <filesystem>


namespace Mix {
	namespace Resource {
		class GltfLoader {
		public:
			bool loadFromGlb(ModelData::Model& _modelData, const std::filesystem::path& _glb, const std::string& _name);

			bool loadFromJson(ModelData::Model&  _modelData, const std::filesystem::path& _json,
			                  const std::string& _name);

		private:
			tinygltf::TinyGLTF mLoader;

			bool getGltfModelFromJson(tinygltf::Model& _model, const std::filesystem::path& _path);

			bool getGltfModelFromGlb(tinygltf::Model& _model, const std::filesystem::path& _path);

			static ModelData::Model LoadModel(const tinygltf::Model& _model, const std::string& _name);

			static void LoadTextureSamplers(const tinygltf::Model& _model, ModelData::Model& _modelData);

			static void LoadTextures(const tinygltf::Model& _model, ModelData::Model& _modelData);

			static void LoadImages(const tinygltf::Model& _model, ModelData::Model& _modelData);

			static void LoadMaterials(const tinygltf::Model& _model, ModelData::Model& _modelData);

			static void LoadMeshes(const tinygltf::Model& _gltfModel, ModelData::Model& _modelData);

			static void ConvertToLeftHanded(Math::Vector3f& _translation, Math::Quaternion& _rotation);

			static void ProcessNode(const tinygltf::Model& _gltfModel,
			                        const tinygltf::Node&  _node,
			                        ModelData::Node&       _parentNode,
			                        ModelData::Model&      _modelData);

			static vk::Filter ToVkFilter(const int32_t _filterMode);

			static vk::SamplerAddressMode ToVkAddressMode(const int32_t _addressMode);
		};
	}
}

#endif

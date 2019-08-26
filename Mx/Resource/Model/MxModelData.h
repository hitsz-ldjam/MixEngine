//#pragma once
//#ifndef MX_MODEL_DATA_H_
//#define MX_MODEL_DATA_H_
//#include "../../Math/MxVector3.h"
//#include "../../Math/MxQuaternion.h"
//#include "../../Vulkan/Core/MxVkDef.h"
//
//#include <vulkan/vulkan.hpp>
//
//#include <cstdint>
//#include <string>
//
//namespace Mix {
//	namespace ModelData {
//		struct Primitive {
//			size_t firstVertex;
//			size_t vertexCount;
//			size_t firstIndex;
//			size_t indexCount;
//
//			int32_t material;
//
//			explicit Primitive(const size_t _firstVertex = 0,
//							   const size_t _vertexCount = 0,
//							   const size_t _firstIndex = 0,
//							   const size_t _indexCount = 0,
//							   const int32_t _material = -1) :
//				firstVertex(_firstVertex),
//				vertexCount(_vertexCount),
//				firstIndex(_firstIndex),
//				indexCount(_indexCount),
//				material(_material) {
//			}
//		};
//
//		struct Mesh {
//			std::string name;
//			std::vector<Primitive> primitives;
//		};
//
//		struct TextureSampler {
//			vk::Filter minFilter = vk::Filter::eLinear;
//			vk::Filter magFilter = vk::Filter::eLinear;
//
//			struct {
//				vk::SamplerAddressMode u = vk::SamplerAddressMode::eClampToEdge;
//				vk::SamplerAddressMode v = vk::SamplerAddressMode::eClampToEdge;
//				vk::SamplerAddressMode w = vk::SamplerAddressMode::eClampToEdge;
//			}addressMode;
//		};
//
//		struct Image {
//			std::string name;
//			uint32_t width = 0;
//			uint32_t height = 0;
//			std::vector<unsigned char> data;
//		};
//
//		struct Texture {
//			int32_t sampler;
//			int32_t image;
//
//			explicit Texture(const int32_t _sampler = -1, const int32_t _image = -1) :sampler(_sampler), image(_image) {}
//		};
//
//		struct Material {
//			enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
//			AlphaMode alphaMode = ALPHAMODE_OPAQUE;
//
//			float alphaCutoff = 1.0f;
//			float metallicFactor = 1.0f;
//			float roughnessFactor = 1.0f;
//
//			Math::Vector4f baseColorFactor = Math::Vector4f::One;
//			Math::Vector4f emissiveFactor = Math::Vector4f::One;
//
//			int32_t baseColorTexture = -1;
//			int32_t metallicRoughnessTexture = -1;
//			int32_t normalTexture = -1;
//			int32_t occlusionTexture = -1;
//			int32_t emissiveTexture = -1;
//
//			struct TexCoordSets {
//				uint8_t baseColor = 0;
//				uint8_t metallicRoughness = 0;
//				uint8_t specularGlossiness = 0;
//				uint8_t normal = 0;
//				uint8_t occlusion = 0;
//				uint8_t emissive = 0;
//			}texCoordSets;
//		};
//
//		struct Node {
//			std::string name;
//
//			// a node will have transform info
//			Math::Vector3f translation;
//			Math::Quaternion rotation;
//			Math::Vector3f scale = Math::Vector3f::One;
//
//			std::optional<std::vector<Node>> children; // a node may have children
//
//			int32_t mesh = -1; // a node may contain a mesh
//		};
//
//		struct Model {
//			std::string name;
//			Node rootNode;
//			std::vector<Mesh> meshes;
//
//			std::vector<Material> materials;
//			std::vector<Texture> textures;
//			std::vector<TextureSampler> textureSamplers;
//
//			std::vector<Vulkan::Vertex> vertices; // mesh data buffer
//			std::vector<uint32_t> indices; // index data buffer
//			std::vector<Image> images; // image data buffer
//		};
//
//	};
//}
//
//#endif

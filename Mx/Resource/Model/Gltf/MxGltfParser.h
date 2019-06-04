#pragma once
#ifndef MX_GLTF_PARSER_H_
#define MX_GLTF_PARSER_H_

#include "../MxModelParserBase.hpp"
#include "../../../Vulkan/MxVkGraphics.h"
#include "../../../Vulkan/Core/MxVkSyncObjMgr.h"
#include "MxGltfLoader.h"

namespace Mix {
	namespace Resource {
		class GltfParser final : public ModelParseBase {
		public:
			GltfParser(): ModelParseBase() {
				mSupportedTypes.insert(ResourceType::GLTF_BIN);
				mSupportedTypes.insert(ResourceType::GLTF_ASCII);

				mSupportedExts.insert("gltf");
				mSupportedExts.insert("glb");

				auto vulkan = Object::FindObjectOfType<Graphics::Vulkan>();
				mFence      = vulkan->getCore()->getSyncObjMgr().createFence();
				vulkan->getLogicalDevice().resetFences(mFence.get());
			}

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type) override;

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext) override;
		private:
			GltfLoader      mLoader;
			Graphics::Fence mFence;

			struct MeshBufferPtrSet {
				std::shared_ptr<Graphics::Buffer> vertex;
				std::shared_ptr<Graphics::Buffer> index;
			};

			Model* modelDataToModel(const ModelData::Model& _modelData);

			MeshBufferPtrSet sendToVulkan(const ModelData::Model& _modelData);

			void buildModel(Model& _resModel, const ModelData::Model& _modelData);

			void recurBuildModelNode(Model::DataType::Node& _node, const ModelData::Node& _dataNode) const;
		};
	}
}

#endif // !MX_GLTF_PARSER_H_

#include "../../../GameObject/MxGameObject.h"
#include "MxGltfParser.h"
#include "../../../Vulkan/MxVkGraphics.h"
#include "../../../Vulkan/CommandBuffer/MxVkCommandPool.h"
#include "../../../Vulkan/Buffers/MxVkBuffer.h"
#include "../../../Utils/MxGuid.h"

namespace Mix {
	namespace Resource {
		std::shared_ptr<ResourceBase> GltfParser::load(const std::filesystem::path& _path, const ResourceType _type) {

			ModelData::Model modelData;
			bool success = false;
			if (_type == ResourceType::GLTF_BIN)
				success = mLoader.loadFromGlb(modelData, _path, _path.filename().string());
			else if (_type == ResourceType::GLTF_ASCII)
				success = mLoader.loadFromJson(modelData, _path, _path.filename().string());

			if (!success)
				return nullptr;

			return std::shared_ptr<ResourceBase>(modelDataToModel(modelData));
		}

		std::shared_ptr<ResourceBase> GltfParser::load(const std::filesystem::path& _path, const std::string& _ext) {
			ModelData::Model modelData;
			bool success = false;
			if (_ext == "glb")
				success = mLoader.loadFromGlb(modelData, _path, _path.filename().string());
			else if (_ext == "gltf")
				success = mLoader.loadFromJson(modelData, _path, _path.filename().string());

			if (!success)
				return nullptr;

			return std::shared_ptr<ResourceBase>(modelDataToModel(modelData));
		}


		Model* GltfParser::modelDataToModel(const ModelData::Model& _modelData) {
			Model* result = new Model();
			buildModel(*result, _modelData);
			return result;
		}

		GltfParser::MeshBufferPtrSet GltfParser::sendToVulkan(const ModelData::Model& _modelData) {
			const auto vulkan = MixEngine::Instance().getModule<Graphics::Vulkan>();
			const auto vkAllocator = vulkan->getAllocator();
			const auto vkDevice = vulkan->getLogicalDevice();
			auto cmd = vulkan->getTransferCommandPool()->allocateUnique();

			// upload vertex data
			vk::DeviceSize byteSize = _modelData.vertices.size() * sizeof(Graphics::Vertex);
			const auto vertexStaging = std::make_shared<Graphics::Buffer>(vkAllocator,
																		  vk::BufferUsageFlagBits::eTransferSrc,
																		  vk::MemoryPropertyFlagBits::eHostVisible |
																		  vk::MemoryPropertyFlagBits::eHostCoherent,
																		  byteSize,
																		  _modelData.vertices.data());

			const auto vertexBuffer = std::make_shared<Graphics::Buffer>(vkAllocator,
																		 vk::BufferUsageFlagBits::eVertexBuffer |
																		 vk::BufferUsageFlagBits::eTransferDst,
																		 vk::MemoryPropertyFlagBits::eDeviceLocal,
																		 byteSize);

			const vk::BufferCopy vertexBufferCopy(0, 0, byteSize);

			// upload index data
			byteSize = _modelData.indices.size() * sizeof(uint32_t);
			const auto indexStaging = std::make_shared<Graphics::Buffer>(vkAllocator,
																		 vk::BufferUsageFlagBits::eTransferSrc,
																		 vk::MemoryPropertyFlagBits::eHostVisible |
																		 vk::MemoryPropertyFlagBits::eHostCoherent,
																		 byteSize,
																		 _modelData.indices.data());

			const auto indexBuffer = std::make_shared<Graphics::Buffer>(vkAllocator,
																		vk::BufferUsageFlagBits::eTransferDst |
																		vk::BufferUsageFlagBits::eIndexBuffer,
																		vk::MemoryPropertyFlagBits::eDeviceLocal,
																		byteSize);

			const vk::BufferCopy indexBufferCopy(0, 0, byteSize);

			cmd->begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
			cmd->copyBuffer(vertexStaging->get(), vertexBuffer->get(), vertexBufferCopy);
			cmd->copyBuffer(indexStaging->get(), indexBuffer->get(), indexBufferCopy);
			cmd->end();

			vk::SubmitInfo submitInfo;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &(*cmd);

			vkDevice->getQueueSet().transfer.value().submit(submitInfo, mFence.get());
			mFence.wait();
			mFence.reset();

			return { vertexBuffer, indexBuffer };
		}

		void GltfParser::buildModel(Model& _resModel, const ModelData::Model& _modelData) {
			auto bufferPtrSet = sendToVulkan(_modelData);
			auto modelDataStruct = GetModeldataStruct(_resModel);

			modelDataStruct->meshes.reserve(_modelData.meshes.size());
			for (auto& meshData : _modelData.meshes) {
				modelDataStruct->meshes.push_back(std::make_shared<Mesh>());
				auto meshDataStruct = GetMeshDataStruct(*modelDataStruct->meshes.back());

				meshDataStruct->vertexBuffer = bufferPtrSet.vertex;
				meshDataStruct->indexBuffer = bufferPtrSet.index;
				meshDataStruct->submeshes.reserve(meshData.primitives.size());

				for (auto& submesh : meshData.primitives)
					meshDataStruct->submeshes.emplace_back(submesh.firstVertex,
														   submesh.vertexCount,
														   submesh.firstIndex,
														   submesh.indexCount);
			}

			recurBuildModelNode(modelDataStruct->rootNode, _modelData.rootNode);
		}

		void GltfParser::recurBuildModelNode(Model::DataType::Node& _node,
											 const ModelData::Node& _dataNode) const {
			_node.translation = _dataNode.translation;
			_node.rotation = _dataNode.rotation;
			_node.scale = _dataNode.scale;
			_node.mesh = _dataNode.mesh;
			_node.name = _dataNode.name;

			if (_dataNode.children) {
				_node.children.emplace();
				_node.children->reserve(_dataNode.children->size());
				for (auto& i : _dataNode.children.value()) {
					Model::DataType::Node node;
					recurBuildModelNode(node, i);
					_node.children->push_back(std::move(node));
				}
			}
		}
	}
}

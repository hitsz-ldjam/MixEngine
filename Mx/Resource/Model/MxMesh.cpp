#include "MxMesh.h"
#include "../../Vulkan/MxVulkan.h"
#include <any>
#include "../../Graphics/MxGraphics.h"
#include <iostream>

namespace Mix {

	void Mesh::setPositions(const std::vector<PositionType>& _vertices) {
		createMeshDataIfNotExist();
		mMeshData->positions = _vertices;
	}

	void Mesh::setPositions(std::vector<PositionType>&& _vertices) {
		createMeshDataIfNotExist();
		mMeshData->positions = std::move(_vertices);
	}

	std::vector<Mesh::PositionType>& Mesh::getPositions() {
		return mMeshData->positions;
	}

	void Mesh::getPositions(std::vector<PositionType>& _vertices) const {
		if (mMeshData)
			_vertices = mMeshData->positions;
	}

	void Mesh::setNormals(const std::vector<NormalType>& _normals) {
		createMeshDataIfNotExist();
		mMeshData->normals = _normals;
	}

	void Mesh::setNormals(std::vector<NormalType>&& _normals) {
		createMeshDataIfNotExist();
		mMeshData->normals = std::move(_normals);
	}

	std::vector<Mesh::NormalType>& Mesh::getNormals() {
		return mMeshData->normals;
	}

	void Mesh::getNormals(std::vector<NormalType>& _normals) const {
		if (mMeshData)
			_normals = mMeshData->normals;
	}

	void Mesh::setTangents(const std::vector<TangentType>& _tangents) {
		createMeshDataIfNotExist();
		mMeshData->tangents = _tangents;
	}

	void Mesh::setTangents(std::vector<TangentType>&& _tangents) {
		createMeshDataIfNotExist();
		mMeshData->tangents = std::move(_tangents);
	}

	std::vector<Mesh::TangentType>& Mesh::getTangents() {
		return mMeshData->tangents;
	}

	void Mesh::getTangents(std::vector<TangentType>& _tangents) const {
		if (mMeshData)
			_tangents = mMeshData->tangents;
	}

	void Mesh::setUVs(UVChannel _channel, const std::vector<UV2DType>& _uvs) {
		createMeshDataIfNotExist();
		switch (_channel) {
		case UVChannel::UV0:
			mMeshData->uv0 = _uvs;
			break;
		case UVChannel::UV1:
			mMeshData->uv1 = _uvs;
			break;
		default:;
		}
	}

	void Mesh::setUVs(UVChannel _channel, std::vector<UV2DType>&& _uvs) {
		createMeshDataIfNotExist();
		switch (_channel) {
		case UVChannel::UV0:
			mMeshData->uv0 = std::move(_uvs);
			break;
		case UVChannel::UV1:
			mMeshData->uv1 = std::move(_uvs);
			break;
		default:;
		}
	}

	std::vector<Mesh::UV2DType>& Mesh::getUVs(UVChannel _channel) {
		switch (_channel) {
		case UVChannel::UV0:return mMeshData->uv0;
		case UVChannel::UV1:return mMeshData->uv1;
		default: assert(false);
		}
	}

	void Mesh::getUVs(UVChannel _channel, std::vector<UV2DType>& _uvs) const {
		if (mMeshData)
			switch (_channel) {
			case UVChannel::UV0:_uvs = mMeshData->uv0;
			case UVChannel::UV1:_uvs = mMeshData->uv1;
			default: assert(false);
			}
	}

	void Mesh::setColors(const std::vector<ColorType>& _colors) {
		createMeshDataIfNotExist();
		mMeshData->colors = _colors;
	}

	void Mesh::setColors(std::vector<ColorType>&& _colors) {
		createMeshDataIfNotExist();
		mMeshData->colors = std::move(_colors);
	}

	std::vector<Mesh::ColorType>& Mesh::getColors() {
		return mMeshData->colors;
	}

	void Mesh::getColors(std::vector<ColorType>& _colors) const {
		if (mMeshData)
			_colors = mMeshData->colors;
	}

	void Mesh::setIndices(const std::vector<uint32_t>& _indices, MeshTopology _topology, uint32_t _submesh, uint32_t _baseVertex) {
		createMeshDataIfNotExist();
		mMeshData->createIndicesAndSubMeshIfNotExist();
		if (_submesh >= mMeshData->subMeshes.value().size()) {
			mMeshData->subMeshes->resize(_submesh + 1);
			mMeshData->indexSet->resize(_submesh + 1);
		}

		mMeshData->indexSet.value()[_submesh] = _indices;
		mMeshData->subMeshes.value()[_submesh].topology = _topology;
		mMeshData->subMeshes.value()[_submesh].baseVertex = _baseVertex;
		mMeshData->subMeshes.value()[_submesh].indexCount = static_cast<uint32_t>(mMeshData->indexSet.value()[_submesh].size());
	}

	void Mesh::setIndices(std::vector<uint32_t>&& _indices, MeshTopology _topology, uint32_t _submesh, uint32_t _baseVertex) {
		createMeshDataIfNotExist();
		mMeshData->createIndicesAndSubMeshIfNotExist();
		if (_submesh >= mMeshData->indexSet->size()) {
			mMeshData->indexSet->resize(_submesh + 1);
			mMeshData->subMeshes->resize(_submesh + 1);
		}

		mMeshData->indexSet.value()[_submesh] = std::move(_indices);
		mMeshData->subMeshes.value()[_submesh].topology = _topology;
		mMeshData->subMeshes.value()[_submesh].baseVertex = _baseVertex;
		mMeshData->subMeshes.value()[_submesh].indexCount = static_cast<uint32_t>(mMeshData->indexSet.value()[_submesh].size());
	}

	std::vector<uint32_t>& Mesh::getIndices(uint32_t _submesh) {
		if (_submesh < mMeshData->indexSet->size())
			return mMeshData->indexSet.value()[_submesh];
		throw Exception("Mesh does not have indices");
	}

	void Mesh::getIndices(std::vector<uint32_t>& _indices, uint32_t _submesh) {
		if (mMeshData && mMeshData->indexSet.has_value() && _submesh < mMeshData->indexSet->size())
			_indices = mMeshData->indexSet.value()[_submesh];
	}

	void Mesh::uploadMeshData(bool _markNoLongerReadable) {
		// Check if has mesh data
		if (!mMeshData)
			return;

		// Check that the number of other attributes is consistent with the number of Vertex
		if (mMeshData->positions.empty()) {
			std::cerr << "Error : Mesh must contain at least position attribute" << std::endl;
			return;
		}
		else {
			bool ok = true;
			if (!mMeshData->normals.empty() && mMeshData->normals.size() != mMeshData->positions.size())
				ok = false;
			if (!mMeshData->tangents.empty() && mMeshData->tangents.size() != mMeshData->positions.size())
				ok = false;
			if (!mMeshData->uv0.empty() && mMeshData->uv0.size() != mMeshData->positions.size())
				ok = false;
			if (!mMeshData->uv1.empty() && mMeshData->uv1.size() != mMeshData->positions.size())
				ok = false;
			if (!mMeshData->colors.empty() && mMeshData->colors.size() != mMeshData->positions.size())
				ok = false;
			if (!ok) {
				std::cerr << "Error : Other attribute must be the same in quantity as position attribute" << std::endl;
				return;
			}
		}

		// Merge vertex data 

		std::vector<std::pair<std::byte*, uint32_t>> srcs;
		uint32_t stride = sizeof(PositionType); // A mesh always has Vertex attribute
		Flags<VertexAttribute> attribute = VertexAttribute::Position;
		srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->positions.data()), sizeof(PositionType));

		if (!mMeshData->normals.empty()) {
			stride += sizeof(NormalType);
			attribute |= VertexAttribute::Normal;
			srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->normals.data()), sizeof(NormalType));
		}
		if (!mMeshData->tangents.empty()) {
			stride += sizeof(TangentType);
			attribute |= VertexAttribute::Tangent;
			srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->tangents.data()), sizeof(TangentType));
		}
		if (!mMeshData->uv0.empty()) {
			stride += sizeof(UV2DType);
			attribute |= VertexAttribute::UV0;
			srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->uv0.data()), sizeof(UV2DType));
		}
		if (!mMeshData->uv1.empty()) {
			stride += sizeof(UV2DType);
			attribute |= VertexAttribute::UV1;
			srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->uv1.data()), sizeof(UV2DType));
		}
		if (!mMeshData->colors.empty()) {
			stride += sizeof(ColorType);
			attribute |= VertexAttribute::Color;
			srcs.emplace_back(reinterpret_cast<std::byte*>(mMeshData->colors.data()), sizeof(ColorType));
		}

		const size_t vertexByteSize = mMeshData->positions.size() * stride;
		std::vector<std::byte> vertexData(vertexByteSize);
		{
			auto ptr = vertexData.data();
			auto count = mMeshData->positions.size();
			for (size_t i = 0; i < count; ++i) {
				for (uint32_t j = 0; j < srcs.size(); ++j) {
					memcpy(ptr, srcs[j].first, srcs[j].second);
					ptr += srcs[j].second;
					srcs[j].first += srcs[j].second;
				}
			}
		}

		// Generate index data

		size_t indexByteSize = 0;
		IndexFormat indexFormat = IndexFormat::UInt16;
		std::vector<std::byte> indexData;
		if (mMeshData->indexSet.has_value()) {
			// Calculate the size of indexData
			indexFormat = mMeshData->positions.size() > std::numeric_limits<uint16_t>::max() ? IndexFormat::UInt32 : IndexFormat::UInt16;
			const auto indexFormatSizeInByte = (indexFormat == IndexFormat::UInt16 ? sizeof(Index16Type) : sizeof(Index32Type));

			uint32_t count = 0;
			for (uint32_t i = 0; i < mMeshData->indexSet->size(); ++i) {
				mMeshData->subMeshes.value()[i].firstIndex = count;
				count += mMeshData->indexSet.value()[i].size();
			}

			indexByteSize = count * indexFormatSizeInByte;

			// Populate index buffer
			indexData.resize(indexByteSize);
			if (indexFormat == IndexFormat::UInt32) {
				size_t offset = 0;
				for (auto index : mMeshData->indexSet.value()) {
					memcpy(indexData.data(), index.data(), index.size() * indexFormatSizeInByte);
					offset += index.size() * indexFormatSizeInByte;
				}
			}
			else {
				auto dst = reinterpret_cast<uint16_t*>(indexData.data());
				for (auto index : mMeshData->indexSet.value()) {
					dst = std::copy(index.begin(), index.end(), dst);
				}
			}
		}

		std::shared_ptr<Vulkan::Buffer> vertexBuffer;
		std::shared_ptr<Vulkan::Buffer> indexBuffer;

		SendToGPU({ vertexByteSize,vertexData.data() }, { indexByteSize,indexData.data() }, vertexBuffer, indexBuffer);

		mVertexBuffer = vertexBuffer;
		mIndexBuffer = indexBuffer;
		mIndexFormat = indexFormat;
		mHasIndex = indexByteSize == 0;
		mAttributes = attribute;
		mSubMeshes = mMeshData->subMeshes.value();
		mVertexDeclaration = std::make_shared<VertexDeclaration>(mAttributes);

		if (_markNoLongerReadable) {
			markNoLongerReadable();
		}
	}

	void Mesh::markNoLongerReadable() {
		mMeshData.reset();
	}

	void Mesh::clear() {
		mIndexBuffer.reset();
		mVertexBuffer.reset();
		mAttributes = Flags<VertexAttribute>();
	}

	std::shared_ptr<Mesh> Mesh::Create(const std::vector<std::byte>& _vertexData,
									   Flags<VertexAttribute> _attributeFlags,
									   const std::vector<std::byte>& _indexData,
									   IndexFormat _format,
									   const std::vector<SubMesh>& _subMeshes) {
		std::shared_ptr<Vulkan::Buffer> vertexBuffer;
		std::shared_ptr<Vulkan::Buffer> indexBuffer;

		if (SendToGPU(_vertexData, _indexData, vertexBuffer, indexBuffer)) {
			std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
			result->mAttributes = _attributeFlags;
			result->mHasIndex = indexBuffer == nullptr;
			result->mSubMeshes = _subMeshes;
			result->mVertexBuffer = vertexBuffer;
			result->mIndexBuffer = indexBuffer;
			result->mVertexDeclaration = std::make_shared<VertexDeclaration>(result->mAttributes);

			return result;
		}

		return nullptr;
	}

	std::shared_ptr<Mesh> Mesh::Create(const std::vector<std::byte>& _vertexData,
									   Flags<VertexAttribute> _attributeFlags,
									   const std::vector<SubMesh>& _subMeshes) {
		std::shared_ptr<Vulkan::Buffer> vertexBuffer;
		std::shared_ptr<Vulkan::Buffer> indexBuffer;

		if (SendToGPU(_vertexData, nullptr, vertexBuffer, indexBuffer)) {
			std::shared_ptr<Mesh> result = std::make_shared<Mesh>();
			result->mAttributes = _attributeFlags;
			result->mHasIndex = indexBuffer == nullptr;
			result->mSubMeshes = _subMeshes;
			result->mVertexBuffer = vertexBuffer;
			result->mIndexBuffer = nullptr;
			result->mVertexDeclaration = std::make_shared<VertexDeclaration>(result->mAttributes);

			return result;
		}

		return nullptr;
	}

	void Mesh::MeshData::createIndicesAndSubMeshIfNotExist() {
		if (!indexSet.has_value())
			indexSet.emplace();
		if (!subMeshes.has_value())
			subMeshes.emplace();
	}

	void Mesh::createMeshDataIfNotExist() {
		if (!mMeshData)
			mMeshData = std::make_shared<MeshData>();
	}

	bool Mesh::SendToGPU(ArrayProxy<const std::byte, vk::DeviceSize> _vertexData,
						 ArrayProxy<const std::byte, vk::DeviceSize> _indexData,
						 std::shared_ptr<Vulkan::Buffer>& _outVertexBuffer,
						 std::shared_ptr<Vulkan::Buffer>& _outIndexBuffer) {
		const auto& vulkan = Graphics::Get()->getRenderApi();
		const auto vkAllocator = vulkan.getAllocator();
		const auto vkDevice = vulkan.getLogicalDevice();
		Vulkan::CommandBufferHandle cmd(vulkan.getTransferCommandPool());

		// upload vertex data
		vk::DeviceSize byteSize = _vertexData.size();
		const vk::BufferCopy vertexBufferCopy(0, 0, byteSize);

		const auto vertexStaging = std::make_shared<Vulkan::Buffer>(vkAllocator,
																	vk::BufferUsageFlagBits::eTransferSrc,
																	vk::MemoryPropertyFlagBits::eHostVisible |
																	vk::MemoryPropertyFlagBits::eHostCoherent,
																	byteSize);

		auto vertexBuffer = std::make_shared<Vulkan::Buffer>(vkAllocator,
															 vk::BufferUsageFlagBits::eVertexBuffer |
															 vk::BufferUsageFlagBits::eTransferDst,
															 vk::MemoryPropertyFlagBits::eDeviceLocal,
															 byteSize);

		// Populate vertex buffer
		std::byte* ptr = static_cast<std::byte*>(vertexStaging->rawPtr());
		memcpy(ptr, _vertexData.data(), byteSize);

		// upload index data if has
		// use uint32_t when there are more than 65535 (max of uint16_t) vertices
		if (!_indexData.empty()) {
			byteSize = _indexData.size();
			const vk::BufferCopy indexBufferCopy(0, 0, byteSize);

			const auto indexStaging = std::make_shared<Vulkan::Buffer>(vkAllocator,
																	   vk::BufferUsageFlagBits::eTransferSrc,
																	   vk::MemoryPropertyFlagBits::eHostVisible |
																	   vk::MemoryPropertyFlagBits::eHostCoherent,
																	   byteSize);

			auto indexBuffer = std::make_shared<Vulkan::Buffer>(vkAllocator,
																vk::BufferUsageFlagBits::eTransferDst |
																vk::BufferUsageFlagBits::eIndexBuffer,
																vk::MemoryPropertyFlagBits::eDeviceLocal,
																byteSize);

	  // populate index buffer
			ptr = static_cast<std::byte*>(indexStaging->rawPtr());
			memcpy(ptr, _indexData.data(), byteSize);


			// send data to gpu
			cmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			cmd.get().copyBuffer(vertexStaging->get(), vertexBuffer->get(), vertexBufferCopy);
			cmd.get().copyBuffer(indexStaging->get(), indexBuffer->get(), indexBufferCopy);
			cmd.end();

			cmd.submit();
			cmd.wait();

			_outIndexBuffer = std::move(indexBuffer);
		}
		else {
			cmd.begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			cmd.get().copyBuffer(vertexStaging->get(), vertexBuffer->get(), vertexBufferCopy);
			cmd.end();

			cmd.submit();
			cmd.wait();
		}

		_outVertexBuffer = std::move(vertexBuffer);
		return true;
	}

	MeshTopology Mesh::getTopology(uint32_t _submesh) const {
		return mSubMeshes[_submesh].topology;
	}
}

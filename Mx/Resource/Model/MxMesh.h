#pragma once
#ifndef MX_MESH_H_
#define MX_MESH_H_
#include <memory>
#include <vector>
#include "../../Vulkan/Buffers/MxVkBuffer.h"
#include "../MxResourceBase.h"
#include "../../Math/MxVector.h"
#include "../../Utils/MxColor.h"
#include "../../Utils/MxArrayProxy.h"
#include "../../Utils/MxFlags.h"

namespace Mix {
	enum class VertexAttribute {
		POSITION = 0x0001,
		NORMAL = 0x0002,
		TANGENT = 0x0004,
		UV0 = 0x0008,
		UV1 = 0x0010,
		COLOR = 0x0020,
		// SKELETON = 0x0040
	};

	MX_ALLOW_FLAGS_FOR_ENUM(VertexAttribute);

	enum class UVChannel {
		UV0 = 0x0008,
		UV1 = 0x0010
	};

	enum class IndexFormat {
		UINT16 = 0x0001,
		UINT32 = 0x0002
	};

	enum class MeshTopology {
		POINTS_LIST = 0x0001,
		LINES_LIST = 0x0002,
		TRIANGLES_LIST = 0x0004
	};

	namespace Vulkan {
		class ShaderBase;
	}

	class Mesh :public ResourceBase {
		friend class Vulkan::ShaderBase;
	public:

		struct SubMesh {
			uint32_t baseVertex = 0;
			uint32_t firstIndex = 0;
			uint32_t indexCount = 0;
			MeshTopology topology = MeshTopology::TRIANGLES_LIST;
		};

		using PositionType = Math::Vector3f;
		using NormalType = Math::Vector3f;
		using TangentType = Math::Vector3f;
		using UV2DType = Math::Vector2f;
		using ColorType = Color32;

		using Index32Type = uint32_t;
		using Index16Type = uint16_t;

		void setPositions(const std::vector<PositionType>& _vertices);
		void setPositions(std::vector<PositionType>&& _vertices);
		std::vector<PositionType>& getPositions();
		void getPositions(std::vector<PositionType>& _vertices) const;

		void setNormals(const std::vector<NormalType>& _normals);
		void setNormals(std::vector<NormalType>&& _normals);
		std::vector<NormalType>& getNormals();
		void getNormals(std::vector<NormalType>& _normals) const;

		void setTangents(const std::vector<TangentType>& _tangents);
		void setTangents(std::vector<TangentType>&& _tangents);
		std::vector<TangentType>& getTangents();
		void getTangents(std::vector<TangentType>& _tangents) const;

		void setUVs(UVChannel _channel, const std::vector<UV2DType>& _uvs);
		void setUVs(UVChannel _channel, std::vector<UV2DType>&& _uvs);
		std::vector<UV2DType>& getUVs(UVChannel _channel);
		void getUVs(UVChannel _channel, std::vector<UV2DType>& _uvs) const;

		void setColors(const std::vector<ColorType>& _colors);
		void setColors(std::vector<ColorType>&& _colors);
		std::vector<ColorType>& getColors();
		void getColors(std::vector<ColorType>& _colors) const;

		void setIndices(const std::vector<uint32_t>& _indices, MeshTopology _topology, uint32_t _submesh, uint32_t _baseVertex = 0);
		void setIndices(std::vector<uint32_t>&& _indices, MeshTopology _topology, uint32_t _submesh, uint32_t _baseVertex = 0);
		std::vector<uint32_t>& getIndices(uint32_t _submesh);
		void getIndices(std::vector<uint32_t>& _indices, uint32_t _submesh);

		void uploadMeshData(bool _markNoLongerReadable);

		void markNoLongerReadable();

		IndexFormat indexFormat() const { return mIndexFormat; }

		bool isReadable() const { return mMeshData.has_value(); }

		uint32_t subMeshCount()const { return static_cast<uint32_t>(mSubMeshes.size()); }

		MeshTopology getTopology(uint32_t _submesh) const;

		void clear();

		bool hasAttributes(Flags<VertexAttribute> _attributesMask) const { return mAttributes.isAllSet(_attributesMask); }

		bool hasIndices() const { return mHasIndex; }

		Flags<VertexAttribute> getAttributesFlags() const { return mAttributes; }

		void setSubMeshes(const std::vector<SubMesh>& _submeshes) { mSubMeshes = _submeshes; }

		void setSubMeshes(std::vector<SubMesh>&& _submeshes) { mSubMeshes = std::move(_submeshes); }


		// ---------- static method ----------

		static std::shared_ptr<Mesh> Create(const std::vector<std::byte>& _vertexData,
											Flags<VertexAttribute> _attributeFlags,
											const std::vector<std::byte>& _indexData,
											IndexFormat _format,
											const std::vector<SubMesh>& _subMeshes);

		static std::shared_ptr<Mesh> Create(const std::vector<std::byte>& _vertexData,
											Flags<VertexAttribute> _attributeFlags,
											const std::vector<SubMesh>& _subMeshes);

	private:

		struct MeshData {
			std::vector<PositionType> positions;
			std::vector<NormalType> normals;
			std::vector<TangentType> tangents;
			std::vector<UV2DType> uv0;
			std::vector<UV2DType> uv1;
			std::vector<ColorType> colors;

			std::optional<std::vector<std::vector<uint32_t>>> indexSet;
			std::optional<std::vector<SubMesh>> subMeshes;

			void createIndicesAndSubMeshIfNotExist();
		};

		//----------- Private field ----------

		Flags<VertexAttribute> mAttributes;
		std::shared_ptr<Vulkan::Buffer> mVertexBuffer;
		bool mHasIndex = false;
		IndexFormat mIndexFormat = IndexFormat::UINT16;
		std::shared_ptr<Vulkan::Buffer> mIndexBuffer;
		std::optional<MeshData> mMeshData;
		std::vector<SubMesh> mSubMeshes;

		// ---------- Private method ----------

		void createMeshDataIfNotExist();

		// ---------- static method ----------

		static bool SendToGPU(ArrayProxy<const std::byte, vk::DeviceSize> _vertexData,
							  ArrayProxy<const std::byte, vk::DeviceSize> _indexData,
							  std::shared_ptr<Vulkan::Buffer>& _outVertexBuffer,
							  std::shared_ptr<Vulkan::Buffer>& _outIndexBuffer);
	};

}

#endif

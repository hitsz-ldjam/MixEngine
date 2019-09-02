#pragma once
#ifndef MX_VERTEX_DECLARATION_H_
#define MX_VERTEX_DECLARATION_H_
#include <cstdint>
#include "../Utils/MxArrayProxy.h"
#include "../Definitions/MxCommonType.h"
#include "../Utils/MxFlags.h"
#include <set>

namespace Mix {
	enum class VertexElementSemantic {
		Position = 0x0001,
		Normal = 0x0002,
		Tangent = 0x0004,
		TexCoord = 0x0008,
		Color = 0x0010,
		Bitangent = 0x0020,
		PointSize = 0x0040,
		Weights = 0x0080,
		Joints = 0x0100
	};

	MX_ALLOW_FLAGS_FOR_ENUM(VertexElementSemantic);

	enum class VertexElementType {
		Unknown = 0,
		Float1,
		Float2,
		Float3,
		Float4,
		Int1,
		Int2,
		Int3,
		Int4,
		UInt1,
		UInt2,
		UInt3,
		UInt4,
		UShort1,
		UShort2,
		UShort3,
		UShort4,
		UByte4_Norm
	};


	class VertexElement {
	public:
		VertexElement() = default;

		VertexElement(uint16_t _streamIndex,
					  uint32_t _location,
					  uint32_t _offset,
					  VertexElementType _type,
					  VertexElementSemantic _semantic,
					  uint16_t _index);

		VertexElement(uint16_t _streamIndex, uint32_t _location, uint32_t _offset, VertexAttribute _vertexAttri);

		uint16_t getStreamIndex() const { return mStreamIndex; }

		uint32_t getLocation() const { return mLocation; }

		uint32_t getOffset() const { return mOffset; }

		VertexElementType getType() const { return mType; }

		VertexElementSemantic getSemantic() const { return mSemantic; }

		uint16_t getSemanticIndex() const { return mIndex; }

		uint16_t getComponentCount() const;

		uint32_t size() const;

		size_t hash() const;

		bool operator==(const VertexElement& _other) const;

		bool operator!=(const VertexElement& _other) const;

		bool operator<(const VertexElement& _other) const;

		bool operator>(const VertexElement& _other) const;

		static uint32_t GetElementTypeSize(VertexElementType _type);

		static uint16_t GetElementComponentCount(VertexElementType _type);

		static VertexElementType GetDefaultTypeForSemantic(VertexElementSemantic _semantic);

		static size_t Hash(const VertexElement& _ve);

	private:
		size_t mHash = 0;
		uint16_t mStreamIndex = 0;
		uint16_t mIndex = 0;
		uint32_t mLocation = 0;
		uint32_t mOffset = 0;
		VertexElementType mType;
		VertexElementSemantic mSemantic;
	};


	class VertexDeclaration {
	public:
		explicit VertexDeclaration(ArrayProxy<const VertexElement> _elements);

		explicit VertexDeclaration(Flags<VertexAttribute> _vertexAttri);

		uint32_t elementCount() const { return static_cast<uint32_t>(mElements.size()); }

		const std::set<VertexElement>& getElements() const { return mElements; }

		const VertexElement* findElementBySemantic(VertexElementSemantic _semantic, uint16_t _index);

		std::vector<VertexElement> getElementsOfStream(uint16_t _streamIndex) const;

		uint32_t getSizeOfStream(uint16_t _streamIndex) const;

		bool isCompatible(const VertexDeclaration& _other) const;

		std::vector<VertexElement> getMissingElements(const VertexDeclaration& _other) const;

		size_t hash() const;

		static size_t Hash(const VertexDeclaration& _vd);

		bool operator==(const VertexDeclaration& _other) const;

		bool operator!=(const VertexDeclaration& _other) const;

	private:
		size_t mHash;
		std::set<VertexElement> mElements;
	};
}

#endif

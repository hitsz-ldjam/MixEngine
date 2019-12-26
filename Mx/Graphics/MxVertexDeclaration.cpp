#include "MxVertexDeclaration.h"
#include "../Utils/MxUtils.h"

namespace Mix {

    const char* ToString(VertexElementSemantic e) {
        switch (e) {
        case VertexElementSemantic::Position: return "Position";
        case VertexElementSemantic::Normal: return "Normal";
        case VertexElementSemantic::Tangent: return "Tangent";
        case VertexElementSemantic::Bitangent: return "Bitangent";
        case VertexElementSemantic::TexCoord: return "TexCoord";
        case VertexElementSemantic::Color: return "Color";
        case VertexElementSemantic::PointSize: return "PointSize";
        case VertexElementSemantic::Weights: return "Weights";
        case VertexElementSemantic::Joints: return "Joints";
        default: return "Unknown";
        }
    }

    VertexElement::VertexElement(uint16_t _streamIndex,
                                 uint32_t _location,
                                 uint32_t _offset,
                                 VertexElementType _type,
                                 VertexElementSemantic _semantic,
                                 uint16_t _index,
                                 uint32_t _instanceRate)
        : mStreamIndex(_streamIndex),
        mIndex(_index),
        mLocation(_location),
        mOffset(_offset),
        mType(_type),
        mSemantic(_semantic),
        mInstanceRate(_instanceRate) {
        mHash = Hash(*this);
    }

    VertexElement::VertexElement(VertexAttribute _vertexAttri, uint16_t _streamIndex, uint32_t _location, uint32_t _offset, uint32_t _instanceRate)
        :mStreamIndex(_streamIndex),
        mIndex(0),
        mLocation(_location),
        mOffset(_offset),
        mInstanceRate(_instanceRate) {
        switch (_vertexAttri) {
        case VertexAttribute::Position:	mSemantic = VertexElementSemantic::Position; break;
        case VertexAttribute::Normal:	mSemantic = VertexElementSemantic::Normal; break;
        case VertexAttribute::Tangent:	mSemantic = VertexElementSemantic::Tangent; break;
        case VertexAttribute::UV0:		mSemantic = VertexElementSemantic::TexCoord; mIndex = 0; break;
        case VertexAttribute::UV1:		mSemantic = VertexElementSemantic::TexCoord; mIndex = 1; break;
        case VertexAttribute::Color:	mSemantic = VertexElementSemantic::Color; break;
        default:;
        }

        mType = GetDefaultTypeForSemantic(mSemantic);
    }

    uint16_t VertexElement::getComponentCount() const {
        return GetElementComponentCount(mType);
    }

    uint16_t VertexElement::GetElementComponentCount(VertexElementType _type) {
        switch (_type) {
        case VertexElementType::Float1:	return 1;
        case VertexElementType::Float2:	return 2;
        case VertexElementType::Float3:	return 3;
        case VertexElementType::Float4:	return 4;
        case VertexElementType::Int1:	return 1;
        case VertexElementType::Int2:	return 2;
        case VertexElementType::Int3:	return 3;
        case VertexElementType::Int4:	return 4;
        case VertexElementType::UInt1:	return 1;
        case VertexElementType::UInt2:	return 2;
        case VertexElementType::UInt3:	return 3;
        case VertexElementType::UInt4:	return 4;
        case VertexElementType::UShort1: return 1;
        case VertexElementType::UShort2: return 2;
        case VertexElementType::UShort3: return 3;
        case VertexElementType::UShort4: return 4;
        case VertexElementType::UByte4_Norm:	return 4;
        default: return 0;
        }
    }

    VertexElementType VertexElement::GetDefaultTypeForSemantic(VertexElementSemantic _semantic) {
        switch (_semantic) {
        case VertexElementSemantic::Position:	return VertexElementType::Float3;
        case VertexElementSemantic::Normal:		return VertexElementType::Float3;
        case VertexElementSemantic::Tangent:	return VertexElementType::Float3;
        case VertexElementSemantic::Bitangent:	return VertexElementType::Float3;
        case VertexElementSemantic::TexCoord:	return VertexElementType::Float2;
        case VertexElementSemantic::Color:		return VertexElementType::UByte4_Norm;
        case VertexElementSemantic::PointSize:	return VertexElementType::Float1;
        case VertexElementSemantic::Weights:	return VertexElementType::Float4;
        case VertexElementSemantic::Joints:		return VertexElementType::UShort4;
        default: return VertexElementType::Unknown;
        }
    }

    size_t VertexElement::Hash(const VertexElement& _ve) {
        size_t result = 0;
        Utils::HashCombine(result, _ve.mStreamIndex);
        Utils::HashCombine(result, _ve.mIndex);
        Utils::HashCombine(result, _ve.mLocation);
        Utils::HashCombine(result, _ve.mOffset);
        Utils::HashCombine(result, _ve.mType);
        Utils::HashCombine(result, _ve.mSemantic);
        Utils::HashCombine(result, _ve.mInstanceRate);
        return result;
    }

    static std::array<VertexAttribute, 6> AllAttributes = {
            VertexAttribute::Position,
            VertexAttribute::Normal,
            VertexAttribute::Tangent,
            VertexAttribute::UV0,
            VertexAttribute::UV1,
            VertexAttribute::Color
    };

    VertexDeclaration::VertexDeclaration(InArrayProxy<const VertexElement> _elements) {
        mElements.insert(_elements.begin(), _elements.end());
        mHash = Hash(*this);
    }

    VertexDeclaration::VertexDeclaration(Flags<VertexAttribute> _vertexAttri) {
        uint32_t location = 0;
        uint32_t offset = 0;
        for (auto& attribute : AllAttributes) {
            if (!_vertexAttri.isSet(attribute))
                continue;

            VertexElement ve(attribute, 0, location, offset, 0);
            ++location;
            offset += VertexElement::GetElementTypeSize(ve.getType());
            mElements.insert(ve);
        }

        mHash = Hash(*this);
    }

    VertexDeclaration::VertexDeclaration(InArrayProxy<std::pair<VertexAttribute, uint32_t>> _attriWithInstanceRate) {
        uint32_t location = 0;
        uint32_t offset = 0;
        for (auto& pair : _attriWithInstanceRate) {
            VertexElement ve(pair.first, 0, location, offset, pair.second);
            ++location;
            offset += VertexElement::GetElementTypeSize(ve.getType());
            mElements.insert(ve);
        }

        mHash = Hash(*this);
    }

    const VertexElement* VertexDeclaration::findElementBySemantic(VertexElementSemantic _semantic, uint16_t _index) {
        for (auto& elem : mElements) {
            if (elem.getSemantic() == _semantic && elem.getSemanticIndex() == _index)
                return &elem;
        }
        return nullptr;
    }

    std::vector<VertexElement> VertexDeclaration::getElementsOfStream(uint16_t _streamIndex) const {
        std::vector<VertexElement> result;
        std::copy_if(mElements.begin(), mElements.end(), result.begin(), [=](const VertexElement& _e) {return _e.getStreamIndex() == _streamIndex; });
        return result;
    }

    uint32_t VertexDeclaration::getSizeOfStream(uint16_t _streamIndex) const {
        uint32_t size = 0;

        for (auto& e : mElements) {
            if (e.getStreamIndex() == _streamIndex)
                size += e.size();
        }

        return size;
    }

    bool VertexDeclaration::isCompatible(const VertexDeclaration& _other) const {
        auto& dst = _other.getElements();
        auto& src = mElements;

        for (auto srcEle : dst) {
            bool found = false;
            for (auto dstEle : src) {
                if (srcEle.getSemantic() == dstEle.getSemantic() &&
                    srcEle.getSemanticIndex() == dstEle.getSemanticIndex() &&
                    srcEle.getStreamIndex() == dstEle.getStreamIndex()) {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }

        return true;
    }

    std::vector<VertexElement> VertexDeclaration::getMissingElements(const VertexDeclaration& _other) const {
        std::vector<VertexElement> result;

        auto& src = _other.getElements();
        auto& dst = mElements;

        for (auto srcEle : src) {
            const VertexElement* foundElement = nullptr;
            for (auto dstEle : dst) {
                if (srcEle.getSemantic() == dstEle.getSemantic() &&
                    srcEle.getSemanticIndex() == dstEle.getSemanticIndex() &&
                    srcEle.getStreamIndex() == dstEle.getStreamIndex()) {
                    foundElement = &dstEle;
                    break;
                }
            }

            if (!foundElement)
                result.push_back(*foundElement);
        }

        return result;
    }

    size_t VertexDeclaration::hash() const {
        return mHash;
    }

    size_t VertexDeclaration::Hash(const VertexDeclaration& _vd) {
        size_t hash = 0;
        for (auto& e : _vd.mElements)
            Utils::HashCombine(hash, e.hash());

        return hash;
    }

    bool VertexDeclaration::operator==(const VertexDeclaration& _other) const {
        return mElements == _other.mElements;
    }

    bool VertexDeclaration::operator!=(const VertexDeclaration& _other) const {
        return !(*this == _other);
    }

    uint32_t VertexElement::size() const {
        return GetElementTypeSize(mType);
    }

    uint32_t VertexElement::GetElementTypeSize(VertexElementType _type) {
        switch (_type) {
        case VertexElementType::Float1: return sizeof(float);
        case VertexElementType::Float2: return sizeof(float) * 2;
        case VertexElementType::Float3: return sizeof(float) * 3;
        case VertexElementType::Float4: return sizeof(float) * 4;
        case VertexElementType::Int1:	return sizeof(int32_t);
        case VertexElementType::Int2:	return sizeof(int32_t) * 2;
        case VertexElementType::Int3:	return sizeof(int32_t) * 3;
        case VertexElementType::Int4:	return sizeof(int32_t) * 4;
        case VertexElementType::UInt1:	return sizeof(uint32_t);
        case VertexElementType::UInt2:	return sizeof(uint32_t) * 2;
        case VertexElementType::UInt3:	return sizeof(uint32_t) * 3;
        case VertexElementType::UInt4:	return sizeof(uint32_t) * 4;
        case VertexElementType::UShort1: return sizeof(uint16_t);
        case VertexElementType::UShort2: return sizeof(uint16_t) * 2;
        case VertexElementType::UShort3: return sizeof(uint16_t) * 3;
        case VertexElementType::UShort4: return sizeof(uint16_t) * 4;
        case VertexElementType::UByte4_Norm: return sizeof(uint8_t) * 4;
        default: return 0;
        }
    }

    size_t VertexElement::hash() const {
        return mHash;
    }

    bool VertexElement::operator==(const VertexElement& _other) const {
        return mStreamIndex == _other.mStreamIndex &&
            mIndex == _other.mIndex &&
            mLocation == _other.mLocation &&
            mOffset == _other.mOffset &&
            mType == _other.mType&&
            mSemantic == _other.mSemantic;

    }

    bool VertexElement::operator!=(const VertexElement& _other) const {
        return !(*this == _other);
    }

    bool VertexElement::operator<(const VertexElement& _other) const {
        return mStreamIndex < _other.mStreamIndex || mStreamIndex == _other.mStreamIndex &&  mLocation < _other.mLocation;
    }

    bool VertexElement::operator>(const VertexElement& _other) const {
        return mStreamIndex > _other.mStreamIndex || mStreamIndex == _other.mStreamIndex &&  mLocation > _other.mLocation;
    }
}

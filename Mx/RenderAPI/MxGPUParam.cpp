#include "MxGPUParam.h"
#include "MxGPUParams.h"

namespace Mix {
    const char* ToString(GPUParamType e) {
        switch (e) {
        case GPUParamType::ParamBlock: return "ParamBlock";
        case GPUParamType::Texture: return "Texture";
        case GPUParamType::Buffer: return "Buffer";
        case GPUParamType::Sampler: return "Sampler";
        default: return "Unknown";
        }
    }

    const char* Mix::ToString(GPUParamDataType e) {
        switch (e) {
        case GPUParamDataType::Bool: return "Bool";
        case GPUParamDataType::Float1: return "Float1";
        case GPUParamDataType::Float2: return "Float2";
        case GPUParamDataType::Float3: return "Float3";
        case GPUParamDataType::Float4: return "Float4";
        case GPUParamDataType::Int1: return "Int1";
        case GPUParamDataType::Int2: return "Int2";
        case GPUParamDataType::Int3: return "Int3";
        case GPUParamDataType::Int4: return "Int4";
        case GPUParamDataType::Color: return "Color";
        case GPUParamDataType::Mat4x4: return "Mat4x4";
        case GPUParamDataType::Struct: return "Struct";
        default: return "Unknown";
        }
    }

    const char* ToString(GPUParamObjectType e) {
        switch (e) {
        case GPUParamObjectType::Sampler1D: return "Sampler1D";
        case GPUParamObjectType::Sampler2D: return "Sampler2D";
        case GPUParamObjectType::Sampler3D: return "Sampler3D";
        case GPUParamObjectType::SamplerCube: return "SamplerCube";
        case GPUParamObjectType::Texture1D: return "Texture1D";
        case GPUParamObjectType::Texture2D: return "Texture2D";
        case GPUParamObjectType::Texture3D: return "Texture3D";
        case GPUParamObjectType::Texture2DMS: return "Texture2DMS";
        case GPUParamObjectType::TextureCube: return "TextureCube";
        case GPUParamObjectType::Texture1DArray: return "Texture1DArray";
        case GPUParamObjectType::Texture2DArray: return "Texture2DArray";
        case GPUParamObjectType::Texture2DMSArray: return "Texture2DMSArray";
        case GPUParamObjectType::TextureCubeArray: return "TextureCubeArray";
        case GPUParamObjectType::RW_Texture1D: return "RW_Texture1D";
        case GPUParamObjectType::RW_Texture2D: return "RW_Texture2D";
        case GPUParamObjectType::RW_Texture3D: return "RW_Texture3D";
        case GPUParamObjectType::RW_Texture2DMS: return "RW_Texture2DMS";
        case GPUParamObjectType::RW_Texture1DArray: return "RW_Texture1DArray";
        case GPUParamObjectType::RW_Texture2DArray: return "RW_Texture2DArray";
        case GPUParamObjectType::RW_Texture2DMSArray: return "RW_Texture2DMSArray";
        case GPUParamObjectType::Buffer: return "Buffer";
        case GPUParamObjectType::RW_Buffer: return "RW_Buffer";
        default: return "Unknown";
        }
    }

    void GPUParamStruct::set(const void* _vaule, uint32 _size, uint32 _arrayIdx) {
    }

    void GPUParamStruct::get(void* _value, uint32 _size, uint32 _arrayIdx) {
    }

    uint32 GPUParamStruct::getElementSize() const {
    }

    void GPUParamTexture::set(const std::shared_ptr<Texture>& _texture) {
        if (mParent == nullptr)
            return;

        mParent->setTexture(mParamDesc->set, mParamDesc->binding, _texture);
        mParent->_markDirty();
    }

    std::shared_ptr<Texture> GPUParamTexture::get() const {
        if (mParent == nullptr)
            return nullptr;

        return mParent->getTexture(mParamDesc->set, mParamDesc->binding);
    }

    void GPUParamBuffer::set(const std::shared_ptr<GPUBuffer>& _buffer) {
        if (mParent == nullptr)
            return;

        mParent->setBuffer(mParamDesc->set, mParamDesc->binding, _buffer);
        mParent->_markDirty();
    }

    std::shared_ptr<GPUBuffer> GPUParamBuffer::get() const {
        if (mParent == nullptr)
            return nullptr;

        return mParent->getBuffer(mParamDesc->set, mParamDesc->binding);
    }

    void GPUParamSampler::set(const SamplerDesc& _samplerDesc) {
        if (mParent == nullptr)
            return;

        mParent->setSampler(mParamDesc->set, mParamDesc->binding, _samplerDesc);
        mParent->_markDirty();
    }

    std::optional<SamplerDesc> GPUParamSampler::get() const {
        if (mParent == nullptr)
            return std::nullopt;

        return mParent->getSampler(mParamDesc->set, mParamDesc->binding);
    }
}

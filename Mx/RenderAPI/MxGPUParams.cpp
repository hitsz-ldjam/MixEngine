#include "MxGPUParams.h"
#include "MxRenderStateManager.h"
#include "../Log/MxLog.h"
#include <numeric>

namespace Mix {
    std::shared_ptr<PipelineParamsInfo> PipelineParamsInfo::Create(const GraphicsParamsDesc& _desc) {
        return RenderStateManager::Get()->createPipelineParamsInfo(_desc);
    }

    std::shared_ptr<PipelineParamsInfo> PipelineParamsInfo::Create(const ComputeParamsDesc& _desc) {
        return RenderStateManager::Get()->createPipelineParamsInfo(_desc);
    }

    PipelineParamsInfo::PipelineParamsInfo(const GraphicsParamsDesc& _desc) {
        std::array<std::shared_ptr<GPUProgramParamDesc>, 6> descs;
        auto[vert, frag, geom, tesse, tessc] = _desc;

        if (vert)
            descs[size_t(GPUProgramType::Vertex)] = vert;
        if (frag)
            descs[size_t(GPUProgramType::Fragment)] = frag;
        if (geom)
            descs[size_t(GPUProgramType::Geometry)] = geom;
        if (tesse)
            descs[size_t(GPUProgramType::TessEvaluation)] = tesse;
        if (tessc)
            descs[size_t(GPUProgramType::TessControl)] = tessc;

        descs[size_t(GPUProgramType::Compute)] = nullptr;

        buildUp(descs);
    }

    PipelineParamsInfo::PipelineParamsInfo(const ComputeParamsDesc& _desc) {
        std::array<std::shared_ptr<GPUProgramParamDesc>, 6> descs;

        descs[size_t(GPUProgramType::Vertex)] = nullptr;
        descs[size_t(GPUProgramType::Fragment)] = nullptr;
        descs[size_t(GPUProgramType::Geometry)] = nullptr;
        descs[size_t(GPUProgramType::TessEvaluation)] = nullptr;
        descs[size_t(GPUProgramType::TessControl)] = nullptr;

        descs[size_t(GPUProgramType::Compute)] = _desc.computeParams;

        buildUp(descs);
    }

    void PipelineParamsInfo::buildUp(const std::array<std::shared_ptr<GPUProgramParamDesc>, 6>& _descs) {
        mParamsDesc = _descs;

        auto countFunc = [&](const auto& _entry, GPUParamType _type) {
            mSetCount = std::max(_entry.set + 1, mSetCount);
            ++mElementCountPerType[size_t(_type)];
            ++mElementCount;
        };

        for (const auto& desc : mParamsDesc) {
            for (const auto& block : desc->paramBlocks)
                countFunc(block.second, GPUParamType::ParamBlock);

            for (const auto& texture : desc->textures)
                countFunc(texture.second, GPUParamType::Texture);

            for (const auto& buffer : desc->buffers)
                countFunc(buffer.second, GPUParamType::Buffer);

            for (const auto& sampler : desc->samplers)
                countFunc(sampler.second, GPUParamType::Sampler);
        }

        std::vector<uint32> bindingCountPerSet(mSetCount, 0);

        for (const auto& desc : mParamsDesc) {
            for (const auto& block : desc->paramBlocks)
                bindingCountPerSet[block.second.set] = std::max(bindingCountPerSet[block.second.set], block.second.binding + 1);

            for (const auto& texture : desc->textures)
                bindingCountPerSet[texture.second.set] = std::max(bindingCountPerSet[texture.second.set], texture.second.binding + 1);

            for (const auto& buffer : desc->buffers)
                bindingCountPerSet[buffer.second.set] = std::max(bindingCountPerSet[buffer.second.set], buffer.second.binding + 1);

            for (const auto& sampler : desc->samplers)
                bindingCountPerSet[sampler.second.set] = std::max(bindingCountPerSet[sampler.second.set], sampler.second.binding + 1);
        }

        // Calculate the size of buffer

        auto setInfoBufferSize = mSetCount * sizeof SetInfo;
        auto bindingInfoBufferSize = sizeof(BindingInfo) * std::accumulate(bindingCountPerSet.begin(), bindingCountPerSet.end(), 0);
        mBuffer.resize(setInfoBufferSize + bindingInfoBufferSize);

        std::byte* pt = mBuffer.data();
        mSetInfos = new (pt) SetInfo[mSetCount];
        pt += setInfoBufferSize;

        for (uint32 i = 0; i < mSetCount; ++i) {
            mSetInfos[i].bindingCount = bindingCountPerSet[i];
            mSetInfos[i].bindingInfo = new (pt) BindingInfo[mSetInfos[i].bindingCount];
            pt += sizeof(BindingInfo) * mSetInfos[i].bindingCount;
        }

        auto populateBindingInfo = [&](auto& _entry, GPUParamType _type) {
            mSetInfos[_entry.set].bindingInfo[_entry.binding].type = _type;
            mSetInfos[_entry.set].bindingInfo[_entry.binding].paramDesc = &_entry;
        };

        for (const auto& desc : mParamsDesc) {
            for (const auto& block : desc->paramBlocks)
                populateBindingInfo(block.second, GPUParamType::ParamBlock);

            for (const auto& texture : desc->textures)
                populateBindingInfo(texture.second, GPUParamType::Texture);

            for (const auto& buffer : desc->buffers)
                populateBindingInfo(buffer.second, GPUParamType::Buffer);

            for (const auto& sampler : desc->samplers)
                populateBindingInfo(sampler.second, GPUParamType::Sampler);
        }
    }

    std::optional<GPUParamLocation> PipelineParamsInfo::getLocation(GPUProgramType _program, GPUParamType _type, const std::string& _name) {
        auto desc = getProgramParamDesc(_program);
        bool found = desc != nullptr;
        GPUParamLocation location;

        if (found) {
            auto f = [&](const auto& _paramMap) -> bool {
                auto it = _paramMap.find(_name);
                if (it != _paramMap.end()) {
                    location.set = it->second.set;
                    location.binding = it->second.binding;
                    return true;
                }
                return false;
            };


            switch (_type) {
            case GPUParamType::ParamBlock:
                found = f(desc->paramBlocks);
                break;
            case GPUParamType::Texture:
                found = f(desc->textures);
                break;
            case GPUParamType::Buffer:
                found = f(desc->buffers);
                break;
            case GPUParamType::Sampler:
                found = f(desc->samplers);
                break;
            default:
                found = false;
            }
        }

        if(found)
            return location;
        else
            return std::nullopt;
    }

    const GPUParamDescBase* PipelineParamsInfo::getParamDesc(uint32 _set, uint32 _binding, GPUParamType _type) const {
#   ifdef MX_DEBUG_MODE

        if (_set >= mSetCount) {
            MX_LOG_ERROR("GPU parameter set index out of range");
            return nullptr;
        }

        if (_set >= mSetInfos[_set].bindingCount) {
            MX_LOG_ERROR("GPU parameter binding index out of range");
            return nullptr;
        }

        auto bindingType = mSetInfos[_set].bindingInfo[_binding].type;
        if (_type != bindingType && bindingType != GPUParamType::Sampler) {
            MX_LOG_ERROR("Requested parameter type does not match. Requested:[%1%], Actual:[%2%]", ToString(_type), ToString(bindingType))
                return nullptr;
        }

#   endif

        return mSetInfos[_set].bindingInfo[_binding].paramDesc;
    }

    std::optional<GPUParamType> PipelineParamsInfo::getParamType(uint32 _set, uint32 _binding) const {
#       ifdef MX_DEBUG_MODE

        if (_set >= mSetCount) {
            MX_LOG_ERROR("GPU parameter set index out of range");
            return std::nullopt;
        }

        if (_set >= mSetInfos[_set].bindingCount) {
            MX_LOG_ERROR("GPU parameter binding index out of range");
            return std::nullopt;
        }

#   endif

        return mSetInfos[_set].bindingInfo[_binding].type;
    }

    std::shared_ptr<GPUProgramParamDesc> PipelineParamsInfo::getProgramParamDesc(GPUProgramType _program) const {
        return mParamsDesc[size_t(_program)];
    }

    ///////////////////////////////////////////////////////////

    std::shared_ptr<GPUProgramParamDesc> GPUParams::getParamDesc(GPUProgramType _type) const {
        return mParamsInfo->getProgramParamDesc(_type);
    }

    bool GPUParams::hasParam(GPUProgramType _type, const std::string& _name) const {
        auto params = mParamsInfo->getProgramParamDesc(_type);
        return params == nullptr ? false : params->paramDatas.count(_name);
    }

    bool GPUParams::hasTexture(GPUProgramType _type, const std::string& _name) const {
        auto params = mParamsInfo->getProgramParamDesc(_type);
        return params == nullptr ? false : params->textures.count(_name);
    }

    bool GPUParams::hasBuffer(GPUProgramType _type, const std::string& _name) const {
        auto params = mParamsInfo->getProgramParamDesc(_type);
        return params == nullptr ? false : params->buffers.count(_name);
    }

    bool GPUParams::hasSampler(GPUProgramType _type, const std::string& _name) const {
        auto params = mParamsInfo->getProgramParamDesc(_type);
        return params == nullptr ? false : params->samplers.count(_name);
    }

    bool GPUParams::hasParamBlock(GPUProgramType _type, const std::string& _name) const {
        auto params = mParamsInfo->getProgramParamDesc(_type);
        return params == nullptr ? false : params->paramBlocks.count(_name);
    }

    void GPUParams::getStructParam(GPUProgramType _program, const std::string& _name, GPUParamStruct& _output) const {
        auto params = mParamsInfo->getProgramParamDesc(_program);
        if (params == nullptr) {
            _output = GPUParamStruct(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Struct] parameter named [%1%]", _name);
            return;
        }

        auto it = params->paramDatas.find(_name);
        if (it == params->paramDatas.end() || it->second.type != GPUParamDataType::Struct) {
            _output = GPUParamStruct(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Struct] parameter named [%1%]", _name);
            return;
        }

        _output = GPUParamStruct(&it->second, const_cast<GPUParams*>(this));
    }

    void GPUParams::getTextureParam(GPUProgramType _program, const std::string& _name, GPUParamTexture& _output) const {
        auto params = mParamsInfo->getProgramParamDesc(_program);
        if (params == nullptr) {
            _output = GPUParamTexture(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Texture] parameter named [%1%]", _name);
            return;
        }

        auto it = params->textures.find(_name);
        if (it == params->textures.end()) {
            _output = GPUParamTexture(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Texture] parameter named [%1%]", _name);
            return;
        }

        _output = GPUParamTexture(&it->second, const_cast<GPUParams*>(this));
    }

    void GPUParams::getBufferParam(GPUProgramType _program, const std::string& _name, GPUParamBuffer& _output) const {
        auto params = mParamsInfo->getProgramParamDesc(_program);
        if (params == nullptr) {
            _output = GPUParamBuffer(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Buffer] parameter named [%1%]", _name);
            return;
        }

        auto it = params->buffers.find(_name);
        if (it == params->buffers.end()) {
            _output = GPUParamBuffer(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Buffer] parameter named [%1%]", _name);
            return;
        }

        _output = GPUParamBuffer(&it->second, const_cast<GPUParams*>(this));
    }

    void GPUParams::getSamplerParam(GPUProgramType _program, const std::string& _name, GPUParamSampler& _output) const {
        auto params = mParamsInfo->getProgramParamDesc(_program);
        if (params == nullptr) {
            _output = GPUParamSampler(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Sampler] parameter named [%1%]", _name);
            return;
        }

        auto it = params->samplers.find(_name);
        if (it == params->samplers.end()) {
            _output = GPUParamSampler(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find [Sampler] parameter named [%1%]", _name);
            return;
        }

        _output = GPUParamSampler(&it->second, const_cast<GPUParams*>(this));
    }

    void GPUParams::setTexture(GPUProgramType _program, const std::string& _name, const std::shared_ptr<Texture>& _texture) {
        GPUParamTexture param;
        getTextureParam(_program, _name, param);
        param.set(_texture);
    }

    void GPUParams::setBuffer(GPUProgramType _program, const std::string& _name, const std::shared_ptr<GPUBuffer>& _buffer) {
        GPUParamBuffer param;
        getBufferParam(_program, _name, param);
        param.set(_buffer);
    }

    void GPUParams::setSampler(GPUProgramType _program, const std::string& _name, const SamplerDesc& _sampler) {
        GPUParamSampler param;
        getSamplerParam(_program, _name, param);
        param.set(_sampler);
    }


    std::shared_ptr<Texture> GPUParams::getTexture(uint32 _set, uint32 _binding) const {
    }

    std::shared_ptr<GPUBuffer> GPUParams::getBuffer(uint32 _set, uint32 _binding) const {
    }

    std::optional<SamplerDesc> GPUParams::getSampler(uint32 _set, uint32 _binding) const {
    }

    void GPUParams::setTexture(uint32 _set, uint32 _binding, const std::shared_ptr<Texture>& _texture) {
    }

    void GPUParams::setBuffer(uint32 _set, uint32 _binding, const std::shared_ptr<GPUBuffer>& _buffer) {
    }

    void GPUParams::setSampler(uint32 _set, uint32 _binding, const SamplerDesc& _sampler) {
    }

    void GPUParams::_markDirty() {
    }

    GPUParams::GPUParams(const std::shared_ptr<PipelineParamsInfo>& _paramsInfo) {
    }




}

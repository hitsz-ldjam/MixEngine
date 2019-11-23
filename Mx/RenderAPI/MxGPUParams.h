#pragma once
#ifndef MX_GPU_PARAMS_H_
#define MX_GPU_PARAMS_H_
#include "../Utils/MxGeneralBase.hpp"
#include "MxGPUParam.h"
#include "MxGPUPipelineState.h"
#include "../Log/MxLog.h"
#include <array>

namespace Mix {
    template<typename _Ty> struct GPUDataParamTypeTraits { enum { TypeId = GPUParamDataType::Struct }; using Type = _Ty; };

    template<> struct GPUDataParamTypeTraits<bool> { enum { TypeId = GPUParamDataType::Bool }; using Type = bool; };

    template<> struct GPUDataParamTypeTraits<float> { enum { TypeId = GPUParamDataType::Float1 }; using Type = float; };
    template<> struct GPUDataParamTypeTraits<Vector2f> { enum { TypeId = GPUParamDataType::Float2 }; using Type = Vector2f; };
    template<> struct GPUDataParamTypeTraits<Vector3f> { enum { TypeId = GPUParamDataType::Float3 }; using Type = Vector3f; };
    template<> struct GPUDataParamTypeTraits<Vector4f> { enum { TypeId = GPUParamDataType::Float4 }; using Type = Vector4f; };

    template<> struct GPUDataParamTypeTraits<int> { enum { TypeId = GPUParamDataType::Int1 }; using Type = int; };
    template<> struct GPUDataParamTypeTraits<Vector2i> { enum { TypeId = GPUParamDataType::Int2 }; using Type = Vector2i; };
    template<> struct GPUDataParamTypeTraits<Vector3i> { enum { TypeId = GPUParamDataType::Int3 }; using Type = Vector3i; };
    template<> struct GPUDataParamTypeTraits<Vector4i> { enum { TypeId = GPUParamDataType::Int4 }; using Type = Vector4i; };

    template<> struct GPUDataParamTypeTraits<Matrix4> { enum { TypeId = GPUParamDataType::Mat4x4 }; using Type = Matrix4; };
    template<> struct GPUDataParamTypeTraits<Color> { enum { TypeId = GPUParamDataType::Color }; using Type = Color; };

    struct GraphicsParamsDesc {
        std::shared_ptr<GPUProgramParamDesc> vertexParams;
        std::shared_ptr<GPUProgramParamDesc> fragmentParams;
        std::shared_ptr<GPUProgramParamDesc> geometryParams;
        std::shared_ptr<GPUProgramParamDesc> tessEvaluationParams;
        std::shared_ptr<GPUProgramParamDesc> tessControlParams;
    };

    struct ComputeParamsDesc {
        std::shared_ptr<GPUProgramParamDesc> computeParams;
    };


    class PipelineParamsInfo {
    public:
        virtual ~PipelineParamsInfo() = default;

        uint32 getSetCount() const { return mSetCount; }

        uint32 getElementCount() const { return mSetCount; }

        uint32 getElementCount(GPUParamType _type) { return mElementCountPerType[size_t(_type)]; }

        std::optional<GPUParamLocation> getLocation(GPUProgramType _program, GPUParamType _type, const std::string& _name);

        const GPUParamDescBase* getParamDesc(uint32 _set, uint32 _binding, GPUParamType _type) const;

        std::optional<GPUParamType> getParamType(uint32 _set, uint32 _binding) const;

        std::shared_ptr<GPUProgramParamDesc> getProgramParamDesc(GPUProgramType _program) const;

        static std::shared_ptr<PipelineParamsInfo> Create(const GraphicsParamsDesc& _desc);

        static std::shared_ptr<PipelineParamsInfo> Create(const ComputeParamsDesc& _desc);

    private:
        friend class RenderStateManager;

        explicit PipelineParamsInfo(const GraphicsParamsDesc& _desc);

        explicit PipelineParamsInfo(const ComputeParamsDesc& _desc);

        void buildUp(const std::array<std::shared_ptr<GPUProgramParamDesc>, 6>& _descs);

    protected:
        /** Used to record the information in a single set */
        struct BindingInfo {
            GPUParamType type;
            const GPUParamDescBase* paramDesc;
        };

        struct SetInfo {
            uint32 bindingCount;
            BindingInfo* bindingInfo;
        };

        std::array<std::shared_ptr<GPUProgramParamDesc>, 6> mParamsDesc;

        uint32 mSetCount = 0;
        SetInfo* mSetInfos = nullptr; /**< Store the info of each binding in each set. */

        uint32 mElementCount = 0;
        std::array<uint32, size_t(GPUParamType::Count)> mElementCountPerType;

        /** We use this as a buffer from which the memory used to store set infos will be allocated */
        std::vector<std::byte> mBuffer;
    };

    class GPUParams :GeneralBase::NoCopyBase {
    public:
        virtual ~GPUParams() = default;

        std::shared_ptr<GPUProgramParamDesc> getParamDesc(GPUProgramType _type) const;

        bool hasParam(GPUProgramType _type, const std::string& _name) const;

        bool hasTexture(GPUProgramType _type, const std::string& _name) const;

        bool hasBuffer(GPUProgramType _type, const std::string& _name) const;

        bool hasSampler(GPUProgramType _type, const std::string& _name) const;

        bool hasParamBlock(GPUProgramType _type, const std::string& _name) const;

        template<typename _Ty> void getParam(GPUProgramType _program, const std::string& _name, TGPUParamData<_Ty>& _output) const;

        void getStructParam(GPUProgramType _program, const std::string& _name, GPUParamStruct& _output) const;

        void getTextureParam(GPUProgramType _program, const std::string& _name, GPUParamTexture& _output) const;

        void getBufferParam(GPUProgramType _program, const std::string& _name, GPUParamBuffer& _output) const;

        void getSamplerParam(GPUProgramType _program, const std::string& _name, GPUParamSampler& _output) const;

        template<typename _Ty>
        void setParam(GPUProgramType _program, const std::string& _name, const _Ty& _input);

        void setTexture(GPUProgramType _program, const std::string& _name, const std::shared_ptr<Texture>& _texture);

        void setBuffer(GPUProgramType _program, const std::string& _name, const std::shared_ptr<GPUBuffer>& _buffer);

        void setSampler(GPUProgramType _program, const std::string& _name, const SamplerDesc& _sampler);

        std::shared_ptr<Texture> getTexture(uint32 _set, uint32 _binding) const;

        std::shared_ptr<GPUBuffer> getBuffer(uint32 _set, uint32 _binding) const;

        std::optional<SamplerDesc> getSampler(uint32 _set, uint32 _binding) const;

        virtual void setTexture(uint32 _set, uint32 _binding, const std::shared_ptr<Texture>& _texture);

        virtual void setBuffer(uint32 _set, uint32 _binding, const std::shared_ptr<GPUBuffer>& _buffer);

        virtual void setSampler(uint32 _set, uint32 _binding, const SamplerDesc& _sampler);

        virtual void _markDirty();

    protected:
        explicit GPUParams(const std::shared_ptr<PipelineParamsInfo>& _paramsInfo);

        std::shared_ptr<PipelineParamsInfo> mParamsInfo;

    };

    template <typename _Ty>
    void GPUParams::getParam(GPUProgramType _program, const std::string& _name, TGPUParamData<_Ty>& _output) const {
        auto params = mParamsInfo->getProgramParamDesc(_program);
        if (params == nullptr) {
            _output = TGPUParamData<_Ty>(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find parameter named [%1%]", _name);
            return;
        }

        auto it = params->paramDatas.find(_name);
        if (it == params->paramDatas.end()) {
            _output = TGPUParamData<_Ty>(nullptr, nullptr);
            MX_LOG_WARNING("Cannot find parameter named [%1%]", _name);
            return;
        }

        _output = TGPUParamData<_Ty>(&it->second, const_cast<GPUParams*>(this));
    }

    template <typename _Ty>
    void GPUParams::setParam(GPUProgramType _program, const std::string& _name, const _Ty& _input) {
        TGPUParamData<_Ty> param;
        getParam(_program, _name, param);
        param.set(_input);
    }
}

#endif

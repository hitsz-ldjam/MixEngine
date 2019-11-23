#pragma once
#ifndef MX_GPU_PARAM_H_
#define MX_GPU_PARAM_H_
#include <string>
#include "../Definitions/MxTypes.h"
#include <map>
#include <utility>
#include "../Definitions/MxCommonEnum.h"
#include "MxSampler.h"
#include "../Math/MxVector2.h"
#include "../Math/MxMatrix4.h"
#include "../Math/MxColor.h"
#include <optional>

namespace Mix {
    enum class GPUParamType {
        ParamBlock,
        Texture,
        Buffer,
        Sampler,
        Count
    };

    const char* ToString(GPUParamType e);

    enum class GPUParamDataType {
        Bool,

        Float1,
        Float2,
        Float3,
        Float4,

        Int1,
        Int2,
        Int3,
        Int4,

        Color,

        /** \todo We now only have built-in matrix type Matrix4f, maybe add more matrix type later */
        // Mat2x2,
        // Mat2x3,
        // Mat2x4,
        // Mat3x2,
        // Mat3x3,
        // Mat3x4,
        // Mat4x2,
        // Mat4x3,
        Mat4x4,

        Struct
    };

    const char* ToString(GPUParamDataType e);

    enum class GPUParamObjectType {
        Sampler1D,
        Sampler2D,
        Sampler3D,
        SamplerCube,

        Texture1D,
        Texture2D,
        Texture3D,
        Texture2DMS,
        TextureCube,

        Texture1DArray,
        Texture2DArray,
        Texture2DMSArray,
        TextureCubeArray,

        RW_Texture1D,
        RW_Texture2D,
        RW_Texture3D,
        RW_Texture2DMS,

        RW_Texture1DArray,
        RW_Texture2DArray,
        RW_Texture2DMSArray,

        Buffer,
        RW_Buffer
    };

    const char* ToString(GPUParamObjectType e);

    /**
     * \brief This structure is used to describe one parameter stored in parameter block.
     *
     * In shader programs, for example GLSL shaders, data parameter such as an int or a vec4 do
     * not normally occupy a single binding point, but are stored in a parameter block with other data parameters.
     * To access one of these parameters, we need to store the information of each parameter.
     */
    struct GPUParamDataDesc {
        std::string name;
        GPUParamDataType type;
        uint32 elementSize;
        uint32 arraySize;
        uint32 arrayElementStride;

        uint32 paramBlockSet;
        uint32 paramBlockBinding;

        uint32 gpuMemOffset;
        uint32 cpuMemOffset;
    };

    /**
     * \brief Base struct for GPUParamxxxDesc, used to describe a parameter of gpu program.
     */
    struct GPUParamDescBase {
    protected:
        GPUParamDescBase() = default;
    };

    /**
     * \brief Contains the information of a data block.
     * \see GPUParamDataDesc
     */
    struct GPUParamBlockDesc :public GPUParamDescBase {
        std::string name;
        uint32 size;
        bool isShareable;

        /** The location of the parameter */
        uint32 set;
        uint32 binding;
    };

    /**
     * \brief Contains the information of an object parameter, usually a sampler or a buffer
     */
    struct GPUParamObjectDesc :public GPUParamDescBase {
        std::string name;
        GPUParamObjectType type; /**< \see GPUParamObjectType */

        /** The location of the parameter */
        uint32 set;
        uint32 binding;
    };

    struct GPUProgramParamDesc {
        std::map<std::string, GPUParamDataDesc> paramDatas;
        std::map<std::string, GPUParamBlockDesc> paramBlocks;

        std::map<std::string, GPUParamObjectDesc> samplers;
        std::map<std::string, GPUParamObjectDesc> textures;
        std::map<std::string, GPUParamObjectDesc> buffers;
        // std::map<std::string, GPUParamObjectDesc> storageBuffers;
    };


    ///////////////////////////////////////////////////////////////
    //                      GPU Parameter                        //
    ///////////////////////////////////////////////////////////////

    class GPUParams;

    template<typename _Ty>
    class TGPUParamData {
    public:
        TGPUParamData() = default;

        TGPUParamData(GPUParamDataDesc* _paramDesc, GPUParams* _parent) :
            mParent(_parent),
            mParamDesc(_paramDesc) {
        }


        void set(const _Ty& _value, uint32 _arrayIdx = 0);

        _Ty get(uint32 _arrayIdx = 0) const;

        const GPUParamDataDesc& getDesc() const { return *mParamDesc; }

        bool operator==(std::nullptr_t) const {
            return mParamDesc == nullptr;
        }

        explicit operator bool() const {
            return mParamDesc != nullptr;
        }

    private:
        GPUParams* mParent;
        GPUParamDataDesc* mParamDesc = nullptr;
    };

    template <typename _Ty>
    void TGPUParamData<_Ty>::set(const _Ty& _value, uint32 _arrayIdx) {
        //todo GPUParamData set(...)
    }

    template <typename _Ty>
    _Ty TGPUParamData<_Ty>::get(uint32 _arrayIdx) const {
    }

    using GPUParamFloat = TGPUParamData<float>;
    using GPUParamVector2f = TGPUParamData<Vector2f>;
    using GPUParamVector3f = TGPUParamData<Vector3f>;
    using GPUParamVector4f = TGPUParamData<Vector4f>;

    using GPUParamInt = TGPUParamData<int>;
    using GPUParamVector2i = TGPUParamData<Vector2i>;
    using GPUParamVector3i = TGPUParamData<Vector3i>;
    using GPUParamVector4i = TGPUParamData<Vector4i>;

    using GPUParamMatrix4 = TGPUParamData<Matrix4>;
    using GPUParamColor = TGPUParamData<Color>;


    class GPUParamStruct {
    public:
        GPUParamStruct() = default;

        GPUParamStruct(GPUParamDataDesc* _paramDesc, GPUParams* _parent) :
            mParent(_parent),
            mParamDesc(_paramDesc) {
        }


        void set(const void* _vaule, uint32 _size, uint32 _arrayIdx = 0);

        void get(void* _value, uint32 _size, uint32 _arrayIdx = 0);

        uint32 getElementSize() const;

        const GPUParamDataDesc& getDesc() const { return *mParamDesc; }

        bool operator==(std::nullptr_t) const {
            return mParamDesc == nullptr;
        }

        explicit operator bool() const {
            return mParamDesc != nullptr;
        }

    private:
        GPUParams* mParent;
        GPUParamDataDesc* mParamDesc = nullptr;
    };


    ///////////////////////////////////////////////////////////////
    //                   GPU object parameters                     //
    ///////////////////////////////////////////////////////////////


    class Texture;
    class GPUBuffer;

    class GPUParamTexture {
    public:
        GPUParamTexture() = default;

        GPUParamTexture(GPUParamObjectDesc* _paramDesc, GPUParams* _parent) :
            mParent(_parent),
            mParamDesc(_paramDesc) {
        }


        void set(const std::shared_ptr<Texture>& _texture);

        std::shared_ptr<Texture> get() const;

        const GPUParamObjectDesc& getDesc() const { return *mParamDesc; }

        bool operator==(std::nullptr_t) const {
            return mParamDesc == nullptr;
        }

        explicit operator bool() const {
            return mParamDesc != nullptr;
        }

    private:
        GPUParams* mParent;
        GPUParamObjectDesc* mParamDesc = nullptr;
    };


    class GPUParamBuffer {
    public:
        GPUParamBuffer() = default;

        GPUParamBuffer(GPUParamObjectDesc* _paramDesc, GPUParams* _parent) :
            mParent(std::move(_parent)),
            mParamDesc(_paramDesc) {
        }

        void set(const std::shared_ptr<GPUBuffer>& _buffer);

        std::shared_ptr<GPUBuffer> get() const;

        const GPUParamObjectDesc& getDesc() const { return *mParamDesc; }

        bool operator==(std::nullptr_t) const {
            return mParamDesc == nullptr;
        }

        explicit operator bool() const {
            return mParamDesc != nullptr;
        }

    private:
        GPUParams* mParent;

        GPUParamObjectDesc* mParamDesc = nullptr;
    };


    class GPUParamSampler {
    public:
        GPUParamSampler() = default;

        GPUParamSampler(GPUParamObjectDesc* _paramDesc, GPUParams* _parent) :
            mParent(std::move(_parent)),
            mParamDesc(_paramDesc) {
        }

        void set(const SamplerDesc& _samplerDesc);

        std::optional<SamplerDesc> get() const;

        const GPUParamObjectDesc& getDesc() const { return *mParamDesc; }

        bool operator==(std::nullptr_t) const {
            return mParamDesc == nullptr;
        }

        explicit operator bool() const {
            return mParamDesc != nullptr;
        }

    private:
        GPUParams* mParent;
        GPUParamObjectDesc* mParamDesc = nullptr;
    };
}

#endif

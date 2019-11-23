#pragma once
#ifndef MX_RESOURCEBASE_H_
#define MX_RESOURCEBASE_H_
#include <string>
#include "../Object/MxObject.h"
#include "../Definitions/MxDefinitions.h"
#include "../Definitions/MxTypes.h"
#include <mutex>

namespace Mix {
    enum class ResourceType {
        Unknown,
        // GLTF Model type
        GLTF_BIN,
        GLTF_ASCII,
        // KTX and DDS texture type
        KTX,
        DDS,
        // GLSL and SPIR-V shader type
        GLSL_VERT,
        GLSL_FRAG,
        GLSL_GEOMETRY,
        GLSL_TESS_CTRL,
        GLSL_TESS_EVLT,
        GLSL_COMPUTE,

        PNG,
        JPG,

        SPIRV_VERT,
        SPIRV_FRAG,
        SPIRV_GEOMETRY,
        SPIRV_TESS_CTRL,
        SPIRV_TESS_EVLT,
        SPIRV_COMPUTE
    };

    inline std::string ToString(ResourceType _resType) {
        switch (_resType) {
        case ResourceType::GLTF_BIN:		return "GLTF_BIN";
        case ResourceType::GLTF_ASCII:		return "GLTF_ASCII";
        case ResourceType::KTX:				return "KTX";
        case ResourceType::DDS:				return "DDS";
        case ResourceType::GLSL_VERT:		return "GLSL_VERT";
        case ResourceType::GLSL_FRAG:		return "GLSL_FRAG";
        case ResourceType::GLSL_GEOMETRY:	return "GLSL_GEOMETRY";
        case ResourceType::GLSL_TESS_CTRL:	return "GLSL_TESS_CTRL";
        case ResourceType::GLSL_TESS_EVLT:	return "GLSL_TESS_EVLT";
        case ResourceType::GLSL_COMPUTE:	return "GLSL_COMPUTE";
        case ResourceType::PNG:				return "PNG";
        case ResourceType::JPG:				return "JPG";
        case ResourceType::SPIRV_VERT:		return "SPIRV_VERT";
        case ResourceType::SPIRV_FRAG:		return "SPIRV_FRAG";
        case ResourceType::SPIRV_GEOMETRY:	return "SPIRV_GEOMETRY";
        case ResourceType::SPIRV_TESS_CTRL: return "SPIRV_TESS_CTRL";
        case ResourceType::SPIRV_TESS_EVLT: return "SPIRV_TESS_EVLT";
        case ResourceType::SPIRV_COMPUTE:	return "SPIRV_COMPUTE";
        default: return "Unknown";
        }
    }

    class ResourceBase {
    public:
        ResourceBase() = default;
        virtual ~ResourceBase() = default;
    };


    template<typename _Target>
    auto GetAdditionalParam();


    //////////////////////////////////////////////////////////////
    //                    New Resource system                   //
    //////////////////////////////////////////////////////////////

    struct ResourceMetaData {

    };


    class Resource : Object {
        MX_DECLARE_RTTI;
    public:
        Resource() = default;

        virtual ~Resource() = 0 {}

        const std::string& getName() const;

        void setName(const std::string& _name);

        virtual bool allowAsyncLoading() const { return true; }

    protected:
        friend class ResourceHandleBase;

        void getResDependencies(std::vector<HResource>& _dependencies) const;

        bool areDependenciesLoaded() const;

        void addResDependency(const HResource& _resource);

        void removeResDependency(const HResource& _resource);

        uint32 mSize;
        bool mKeepData;
        std::string mName;
        std::vector<HResource> mDependencies;
        mutable std::mutex mDependencyMutex;
    };

}

#endif

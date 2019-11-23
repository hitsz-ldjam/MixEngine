#pragma once
#ifndef MX_GPU_PROGRAM_H_
#define MX_GPU_PROGRAM_H_

#include "../Definitions/MxCommonEnum.h"
#include <string>
#include <vector>

namespace Mix {
    struct GPUProgramDesc {
        std::string source;
        std::string entryPoint;
        std::string language;
        GPUProgramType type = GPUProgramType::Vertex;
        bool adjacencyRequired = false;

        std::vector<std::byte> binaryCode;
    };

    class GPUProgramParamDesc;
    class VertexDeclaration;

    class GPUProgram {
    public:
        struct CompileStatus {
            bool success = false;
            std::string message;
        };

        virtual bool isCompiled() const = 0;

        virtual std::string getCompileMessage() const = 0;

        virtual bool isSupport() const = 0;

        bool isAdjacencyRequired() const { return mDesc.adjacencyRequired; }

        GPUProgramType getProgramType() const { return mDesc.type; }

        std::shared_ptr<GPUProgramParamDesc> getParamDesc() const { return mParamDesc; }

        std::shared_ptr<VertexDeclaration> getVertexDeclaration() const { return mVertexDecl; }

        static std::shared_ptr<GPUProgram> Create(const GPUProgramDesc& _desc);

    protected:
        GPUProgram(const GPUProgramDesc& _desc) :mDesc(_desc) {}

        GPUProgramDesc mDesc;
        CompileStatus mCompileStatus;
        std::shared_ptr<GPUProgramParamDesc> mParamDesc;
        std::shared_ptr<VertexDeclaration> mVertexDecl;
    };
}

#endif

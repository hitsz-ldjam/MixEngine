#pragma once
#ifndef MX_GPU_PROGRAM_MANAGER_H_
#define MX_GPU_PROGRAM_MANAGER_H_
#include <memory>
#include "MxGPUProgram.h"
#include <unordered_map>

namespace Mix {
    class GPUProgram;

    class GPUProgramFactory {
    public:
        virtual ~GPUProgramFactory() = default;

        virtual std::shared_ptr<GPUProgram> create(const GPUProgramDesc& _desc) = 0;

        virtual std::string getSupportedLanguage() const = 0;
    };


    class GPUProgramManager {
    public:
        void addFactory(std::shared_ptr<GPUProgramFactory> _factory);

        void removeFactory(const std::string& _language);

        std::shared_ptr<GPUProgramFactory> getFactory(const std::string& _language) const;

        bool isLanguageSupported(const std::string& _language) const;

        std::shared_ptr<GPUProgram> create(const GPUProgramDesc& _desc) const;

    private:
        std::unordered_map<std::string, std::shared_ptr<GPUProgramFactory>> mFactories;

    };
}

#endif

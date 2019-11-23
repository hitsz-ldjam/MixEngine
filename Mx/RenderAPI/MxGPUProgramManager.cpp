#include "MxGPUProgramManager.h"

namespace Mix {
    void GPUProgramManager::addFactory(std::shared_ptr<GPUProgramFactory> _factory) {
        if (_factory) {
            mFactories[_factory->getSupportedLanguage()] = std::move(_factory);
        }
    }

    void GPUProgramManager::removeFactory(const std::string& _language) {
        auto it = mFactories.find(_language);
        if (it != mFactories.end())
            mFactories.erase(it);
    }

    std::shared_ptr<GPUProgramFactory> GPUProgramManager::getFactory(const std::string& _language) const {
        auto it = mFactories.find(_language);
        if (it != mFactories.end())
            return it->second;
        return nullptr;
    }

    bool GPUProgramManager::isLanguageSupported(const std::string& _language) const {
        return mFactories.count(_language);
    }

    std::shared_ptr<GPUProgram> GPUProgramManager::create(const GPUProgramDesc& _desc) const {
        auto factory = getFactory(_desc.language);
        return factory == nullptr ? nullptr : factory->create(_desc);
    }
}

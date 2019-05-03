#include "MxVkShader.h"

namespace Mix {
    namespace Graphics {
        void ShaderMgr::destroy() {
            if (!mCore)
                return;

            for (auto& pair : mModules) {
                mCore->GetDevice().destroyShaderModule(pair.second.module);
            }
            mModules.clear();
            mCore = nullptr;
        }

        void ShaderMgr::createShader(const std::string & name, const char * data, const size_t size, vk::ShaderStageFlagBits stage) {
            if (mModules.count(name) != 0)
                throw ShaderAlreadyExist(name);

            vk::ShaderModule temp;
            vk::ShaderModuleCreateInfo createInfo;
            createInfo.codeSize = size;
            createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

            temp = mCore->GetDevice().createShaderModule(createInfo);
            mModules.insert(std::make_pair(name, ShaderModule(temp, stage)));
        }

        const ShaderModule & ShaderMgr::getModule(const std::string & name) {
            if (mModules.count(name) == 0)
                throw ShaderNotFound(name);

            return mModules[name];
        }
        void ShaderMgr::destroyModule(const std::string & name) {
            if (mModules.count(name) == 0)
                throw ShaderNotFound(name);

            mCore->GetDevice().destroyShaderModule(mModules[name].module);
        }
    }
}
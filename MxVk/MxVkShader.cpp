#include "MxVkShader.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(ShaderMgr, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(ShaderMgr);

        void ShaderMgr::destroy() {
            for (auto& pair : mModules) {
                mCore->device().destroyShaderModule(pair.second.module);
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

            temp = mCore->device().createShaderModule(createInfo);
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

            mCore->device().destroyShaderModule(mModules[name].module);
        }
    }
}
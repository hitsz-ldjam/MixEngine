#include "MxVkShader.h"

#include "../Core/MxVkExcept.hpp"

namespace Mix {
    namespace Graphics {
        void ShaderMgr::destroy() {
            if (!mCore)
                return;

            for (auto& pair : mModules) {
                mCore->getDevice().destroyShaderModule(pair.second.module);
            }
            mModules.clear();
            mCore = nullptr;
        }

        void ShaderMgr::createShader(const std::string & _name, const char * _data, const size_t _size, vk::ShaderStageFlagBits _stage) {
            if (mModules.count(_name) != 0)
                throw ShaderAlreadyExist(_name);

            vk::ShaderModuleCreateInfo createInfo;
            createInfo.codeSize = _size;
            createInfo.pCode = reinterpret_cast<const uint32_t*>(_data);

            const vk::ShaderModule temp = mCore->getDevice().createShaderModule(createInfo);
            mModules.insert(std::make_pair(_name, ShaderModule(temp, _stage)));
        }

        const ShaderModule & ShaderMgr::getModule(const std::string & _name) {
            if (mModules.count(_name) == 0)
                throw ShaderNotFound(_name);

            return mModules[_name];
        }
        void ShaderMgr::destroyModule(const std::string & _name) {
            if (mModules.count(_name) == 0)
                throw ShaderNotFound(_name);

            mCore->getDevice().destroyShaderModule(mModules[_name].module);
        }
    }
}

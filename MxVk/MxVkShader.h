#pragma once
#ifndef _MX_VK_SHADER_H_
#define _MX_VK_SHADER_H_


#include "MxVkCore.h"
#include <string>
#include <unordered_map>
#include <map>


namespace Mix {
    namespace Graphics {
        struct ShaderModule {
            vk::ShaderModule module;
            vk::ShaderStageFlagBits stage;

            ShaderModule() = default;
            ShaderModule(const vk::ShaderModule m, const vk::ShaderStageFlagBits s) {
                module = m;
                stage = s;
            }

            bool operator==(const ShaderModule& shader) {
                return module == shader.module;
            }
        };


        class ShaderMgr :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~ShaderMgr() { destroy(); };

            void destroy();

            void createShader(const std::string& name, const char* data, const size_t size,vk::ShaderStageFlagBits stage);

            const ShaderModule& getModule(const std::string& name);

            void destroyModule(const std::string& name);

        private:
            std::unordered_map<std::string, ShaderModule> mModules;
        };
    }
}
#endif // !_MX_VK_SHADER_H_

#pragma once
#ifndef MX_VK_SHADER_H_
#define MX_VK_SHADER_H_

#include "MxVkCore.h"
#include <string>
#include <unordered_map>


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

            bool operator==(const ShaderModule& _shader) const {
                return module == _shader.module;
            }

            bool operator!=(const ShaderModule& _shader)const {
                return !(*this == _shader);
            }
        };


        class ShaderMgr :public GraphicsComponent {
        public:
            ~ShaderMgr() { destroy(); };

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            void destroy();

            void createShader(const std::string& name, const char* data, const size_t size, vk::ShaderStageFlagBits stage);

            const ShaderModule& getModule(const std::string& name);

            void destroyModule(const std::string& name);

        private:
            std::unordered_map<std::string, ShaderModule> mModules;
        };
    }
}
#endif // !MX_VK_SHADER_H_

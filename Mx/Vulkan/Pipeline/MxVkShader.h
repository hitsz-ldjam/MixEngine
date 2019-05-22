#pragma once
#ifndef MX_VK_SHADER_H_
#define MX_VK_SHADER_H_

#include "../Core/MxVkCore.h"
#include <string>
#include <unordered_map>


namespace Mix {
    namespace Graphics {
        struct ShaderModule {
            vk::ShaderModule module;
            vk::ShaderStageFlagBits stage;

            ShaderModule() = default;
            ShaderModule(const vk::ShaderModule _module, const vk::ShaderStageFlagBits _stage) :
                module(_module),
                stage(_stage) {
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

            void createShader(const std::string& _name,
                              const char* _data,
                              const size_t _size,
                              vk::ShaderStageFlagBits _stage);

            const ShaderModule& getModule(const std::string& _name);

            void destroyModule(const std::string& _name);

        private:
            std::unordered_map<std::string, ShaderModule> mModules;
        };
    }
}
#endif // !MX_VK_SHADER_H_

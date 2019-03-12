#pragma once
#ifndef _MX_VK_EXCEPT_HPP_
#define _MX_VK_EXCEPT_HPP_

#include <stdexcept>


namespace Mix {
    namespace Graphics {
        class NoSuitableVulkanDevice :public std::runtime_error {
        public:
            NoSuitableVulkanDevice() :runtime_error("Error : Failed to find device with Vulkan support") {};
        };

        class SurfaceFormatUnsupported :public std::runtime_error {
        public:
            SurfaceFormatUnsupported() :runtime_error("Error : Surface format isn't supported by Vulkan device") {};
        };

        class PresentModeUnsupported :public std::runtime_error {
        public:
            PresentModeUnsupported() :runtime_error("Error : Present mode isn't supported by Vulkan device") {};
        };

        class ShaderAlreadyExist :public std::runtime_error {
        public:
            ShaderAlreadyExist(const std::string& name) :runtime_error("") {
                mWhat = "Error : Shader [" + name + "] already exists";
            }

            const char* what() const override {
                return mWhat.c_str();
            }

        private:
            std::string mWhat;
        };

        class ShaderNotFound :public std::runtime_error {
        public:
            ShaderNotFound(const std::string& name) :runtime_error("") {
                mWhat = "Error : Shader [" + name + "] not found";
            }

            const char* what() const override {
                return mWhat.c_str();
            }

        private:
            std::string mWhat;
        };

        class SyncObjNotFound :public std::runtime_error {
        public:
            SyncObjNotFound() :runtime_error("Error : Sync object not found") {};
        };

        class PipelineAlreadyExist :public std::runtime_error {
        public:
            PipelineAlreadyExist(const std::string& name) :runtime_error("") {
                mWhat = "Error : Pipeline [" + name + "] already exists";
            }

            const char* what() const override {
                return mWhat.c_str();
            }

        private:
            std::string mWhat;
        };

        class PipelineNotFound :public std::runtime_error {
        public:
            PipelineNotFound(const std::string& name) :runtime_error("") {
                mWhat = "Error : Pipeline [" + name + "] not found";
            }

            const char* what() const override {
                return mWhat.c_str();
            }

        private:
            std::string mWhat;
        };

        class SwapchainSwapFailed :public std::runtime_error {
        public:
            SwapchainSwapFailed() :runtime_error("Error : Failed to acquire swapchain image") {
            }
        };
    }
}
#endif // !_MX_VK_EXCEPT_HPP_

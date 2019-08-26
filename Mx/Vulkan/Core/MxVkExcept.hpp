#pragma once
#ifndef MX_VK_EXCEPT_HPP_
#define MX_VK_EXCEPT_HPP_

#include <stdexcept>


namespace Mix {
    namespace Vulkan {
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
            explicit ShaderAlreadyExist(const std::string& name) :runtime_error("") {
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
            explicit ShaderNotFound(const std::string& _name) :runtime_error("") {
                mWhat = "Error : Shader [" + _name + "] not found";
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
            explicit PipelineAlreadyExist(const std::string& _name) :runtime_error("") {
                mWhat = "Error : Pipeline [" + _name + "] already exists";
            }

            const char* what() const override {
                return mWhat.c_str();
            }

        private:
            std::string mWhat;
        };

        class PipelineNotFound :public std::runtime_error {
        public:
            explicit PipelineNotFound(const std::string& _name) :runtime_error("") {
                mWhat = "Error : Pipeline [" + _name + "] not found";
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
#endif // !MX_VK_EXCEPT_HPP_

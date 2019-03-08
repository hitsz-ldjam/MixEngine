#pragma once
#ifndef _MX_VK_EXCEPT_HPP_
#define _MX_VK_EXCEPT_HPP_

#include <stdexcept>


namespace Mix {
    namespace Graphics {
        class NoSuitableVulkanDevice :public std::runtime_error {
        public:
            NoSuitableVulkanDevice() :runtime_error("") {};

            const char* what() const override {
                return "Error : Failed to find device with Vulkan support";
            }
        };
    }
}
#endif // !_MX_VK_EXCEPT_HPP_

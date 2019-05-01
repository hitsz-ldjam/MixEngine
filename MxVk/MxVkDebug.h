#pragma once
#ifndef _MX_VK_DEBUG_H_
#define _MX_VK_DEBUG_H_


#include "MxVkCore.h"
#include <vector>
#include <algorithm>
#include <string>


namespace Mix {
    namespace Graphics {
        class Debug :public GraphicsComponent {
        public:
            Debug() { mCore = nullptr; };
            virtual ~Debug() { destroy(); }

            const vk::DebugUtilsMessengerEXT& setDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& messageSeverity,
                                                               const vk::DebugUtilsMessageTypeFlagsEXT& messageType,
                                                               PFN_vkDebugUtilsMessengerCallbackEXT callback,
                                                               void* userData);

            const vk::DebugUtilsMessengerEXT& setDefaultCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
                                                                 const vk::DebugUtilsMessageTypeFlagsEXT& messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) {


                return setDebugCallback(messageSeverity, messageType, debugCallback, nullptr);
            }

            void destroyDebugCallback(const vk::DebugUtilsMessengerEXT& messenger);

            void destroy();

            static std::string severityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity);
            static std::string typeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT type);

        private:
            std::vector<vk::DebugUtilsMessengerEXT> mMessengers;

            static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void* pUserData);
        };
    }
}

#endif // !_MX_VK_DEBUG_H_

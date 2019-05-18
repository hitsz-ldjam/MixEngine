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

            ~Debug() { destroy(); }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

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


                return setDebugCallback(messageSeverity, messageType, DebugCallback, nullptr);
            }

            void destroyDebugCallback(const vk::DebugUtilsMessengerEXT& _messenger);

            void destroy();

            static std::string SeverityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT _severity);
            static std::string TypeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT _type);

        private:
            std::vector<vk::DebugUtilsMessengerEXT> mMessengers;

            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT _messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
                                                                void* _pUserData);
        };
    }
}

#endif // !_MX_VK_DEBUG_H_

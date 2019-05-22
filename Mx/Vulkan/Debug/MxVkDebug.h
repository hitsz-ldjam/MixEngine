#pragma once
#ifndef _MX_VK_DEBUG_H_
#define _MX_VK_DEBUG_H_


#include "../Core/MxVkCore.h"
#include <vector>
#include <string>


namespace Mix {
    namespace Graphics {
        class DebugUtils :public GraphicsComponent {
        public:
            DebugUtils() { mCore = nullptr; };

            ~DebugUtils() { destroy(); }

            void init(const std::shared_ptr<Core>& _core) {
                setCore(_core);
            }

            const vk::DebugUtilsMessengerEXT& setDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& _messageSeverity,
                                                               const vk::DebugUtilsMessageTypeFlagsEXT& _messageType,
                                                               PFN_vkDebugUtilsMessengerCallbackEXT _callback,
                                                               void* _userData);

            const vk::DebugUtilsMessengerEXT& setDefaultCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& _messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
                                                                 const vk::DebugUtilsMessageTypeFlagsEXT& _messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) {


                return setDebugCallback(_messageSeverity, _messageType, DebugCallback, nullptr);
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

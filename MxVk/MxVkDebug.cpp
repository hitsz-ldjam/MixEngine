#include "MxVkDebug.h"

namespace Mix {
    namespace Graphics {
        const vk::DebugUtilsMessengerEXT& Debug::setDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT & messageSeverity, const vk::DebugUtilsMessageTypeFlagsEXT & messageType, PFN_vkDebugUtilsMessengerCallbackEXT callback, void * userData) {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo;
            createInfo.messageSeverity = messageSeverity;
            createInfo.messageType = messageType;
            createInfo.pfnUserCallback = callback;

            vk::DebugUtilsMessengerEXT messenger = mCore->getInstance().createDebugUtilsMessengerEXT(createInfo,
                                                                                                  nullptr,
                                                                                                  mCore->dynamicLoader());

            mMessengers.push_back(std::move(messenger));
            return mMessengers.back();
        }

        void Debug::destroyDebugCallback(const vk::DebugUtilsMessengerEXT & _messenger) {
            mCore->getInstance().destroyDebugUtilsMessengerEXT(_messenger, nullptr, mCore->dynamicLoader());
            mMessengers.erase(std::find(mMessengers.begin(),
                              mMessengers.end(),
                              _messenger));
        }

        void Debug::destroy() {
            if (!mCore)
                return;

            for (const auto& messenger : mMessengers)
                mCore->getInstance().destroyDebugUtilsMessengerEXT(messenger, nullptr, mCore->dynamicLoader());

            mMessengers.clear();
            mCore = nullptr;
        }

        std::string Debug::SeverityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT _severity) {
            switch (_severity) {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                return "Verbose";
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
                return "Info";
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                return "Warning";
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                return "Error";
            default:
                return "Unknown";
            }
        }

        std::string Debug::TypeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT _type) {
            switch (_type) {
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
                return "General";
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
                return "Validation";
            case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
                return "Performance";
            default:
                return "Unkown";
            }
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Debug::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, VkDebugUtilsMessageTypeFlagsEXT _messageType, const VkDebugUtilsMessengerCallbackDataEXT * _pCallbackData, void * _pUserData) {
            std::string msg = "[ Validation layer ] : [ ";
            msg += SeverityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(_messageSeverity));
            msg += " ] [ ";
            msg += TypeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(_messageType));
            msg = msg + " ]\n\t" + _pCallbackData->pMessage;
            std::cerr << std::endl << msg << std::endl;
            return VK_FALSE;
        }

    }
}

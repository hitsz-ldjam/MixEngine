#include "MxVkDebug.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Debug, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Debug);

        const vk::DebugUtilsMessengerEXT& Debug::setDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT & messageSeverity, const vk::DebugUtilsMessageTypeFlagsEXT & messageType, PFN_vkDebugUtilsMessengerCallbackEXT callback, void * userData) {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo;
            createInfo.messageSeverity = messageSeverity;
            createInfo.messageType = messageType;
            createInfo.pfnUserCallback = callback;

            vk::DebugUtilsMessengerEXT messenger = mCore->instance().createDebugUtilsMessengerEXT(createInfo,
                                                                                                  nullptr,
                                                                                                  mCore->dynamicLoader());

            mMessengers.push_back(std::move(messenger));
            return mMessengers.back();
        }

        void Debug::destroyDebugCallback(const vk::DebugUtilsMessengerEXT & messenger) {
            mCore->instance().destroyDebugUtilsMessengerEXT(messenger, nullptr, mCore->dynamicLoader());
            mMessengers.erase(std::find(mMessengers.begin(),
                              mMessengers.end(),
                              messenger));
        }

        void Debug::destroy() {
            for (const auto& messenger : mMessengers)
                mCore->instance().destroyDebugUtilsMessengerEXT(messenger, nullptr, mCore->dynamicLoader());

            mMessengers.clear();
            mCore = nullptr;
        }

        std::string Debug::severityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity) {
            switch (severity) {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                return "Verbose";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
                return "Info";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                return "Warning";
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                return "Error";
                break;
            default:
                return "Unknown";
                break;
            }
        }

        std::string Debug::typeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT type) {
            switch (type) {
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral:
                return "General";
                break;
            case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation:
                return "Validation";
                break;
            case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance:
                return "Performance";
                break;
            default:
                return "Unkown";
                break;
            }
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL Debug::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData, void * pUserData) {
            std::string msg = "[ Validation layer ] : [ ";
            msg += severityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity));
            msg += " ] [ ";
            msg += typeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType));
            msg = msg + " ]\n\t" + pCallbackData->pMessage;
            std::cerr << std::endl << msg << std::endl;
            return VK_FALSE;
        }

    }
}

#include "MxVkDebug.h"
#include "../../Log/MxLog.h"

namespace Mix {
    namespace Vulkan {
	    DebugUtils::~DebugUtils() {
			if (!mDevice)
				return;

			for (const auto& messenger : mMessengers)
				mInstance->get().destroyDebugUtilsMessengerEXT(messenger, nullptr, mDevice->getDynamicLoader());
	    }

        const vk::DebugUtilsMessengerEXT& DebugUtils::addDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT & _messageSeverity, const vk::DebugUtilsMessageTypeFlagsEXT & _messageType, PFN_vkDebugUtilsMessengerCallbackEXT _callback, void * _userData) {
            vk::DebugUtilsMessengerCreateInfoEXT createInfo;
            createInfo.messageSeverity = _messageSeverity;
            createInfo.messageType = _messageType;
            createInfo.pfnUserCallback = _callback;

            vk::DebugUtilsMessengerEXT messenger = mInstance->get().createDebugUtilsMessengerEXT(createInfo,
                                                                                                  nullptr,
                                                                                                  mDevice->getDynamicLoader());

            mMessengers.push_back(messenger);
            return mMessengers.back();
        }

        void DebugUtils::destroyDebugCallback(const vk::DebugUtilsMessengerEXT & _messenger) {
            mInstance->get().destroyDebugUtilsMessengerEXT(_messenger, nullptr, mDevice->getDynamicLoader());
            mMessengers.erase(std::find(mMessengers.begin(),
                              mMessengers.end(),
                              _messenger));
        }

        std::string DebugUtils::SeverityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT _severity) {
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

        std::string DebugUtils::TypeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT _type) {
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

        VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtils::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
                                                                 VkDebugUtilsMessageTypeFlagsEXT _messageType,
                                                                 const VkDebugUtilsMessengerCallbackDataEXT * _pCallbackData, 
                                                                 void * _pUserData) {
            switch (_messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                Mix::Debug::Log::Info("[Validation layer]:[%s|%s]: %s\n",
                                      SeverityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(_messageSeverity)).c_str(),
                                      TypeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(_messageType)).c_str(),
                                      _pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Mix::Debug::Log::Warning("[Validation layer]:[%s|%s]: %s\n",
                                      SeverityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(_messageSeverity)).c_str(),
                                      TypeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(_messageType)).c_str(),
                                      _pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Mix::Debug::Log::Error("[Validation layer]:[%s|%s]: %s\n",
                                      SeverityToString(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(_messageSeverity)).c_str(),
                                      TypeToString(static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(_messageType)).c_str(),
                                      _pCallbackData->pMessage);
                break;
            }
            return VK_FALSE;
        }

    }
}

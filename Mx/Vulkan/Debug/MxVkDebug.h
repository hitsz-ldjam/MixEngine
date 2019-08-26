#pragma once
#ifndef _MX_VK_DEBUG_H_
#define _MX_VK_DEBUG_H_


#include "../Device/MxVkDevice.h"
#include <vector>
#include <string>


namespace Mix {
	namespace Vulkan {
		class DebugUtils :public GeneralBase::NoCopyBase {
		public:
			explicit DebugUtils(const std::shared_ptr<Device>& _device) :mInstance(_device->getPhysicalDevice()->getInstance()), mDevice(_device) {}

			~DebugUtils();

			const vk::DebugUtilsMessengerEXT& addDebugCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& _messageSeverity,
															   const vk::DebugUtilsMessageTypeFlagsEXT& _messageType,
															   PFN_vkDebugUtilsMessengerCallbackEXT _callback,
															   void* _userData);

			const vk::DebugUtilsMessengerEXT& addDefaultCallback(const vk::DebugUtilsMessageSeverityFlagsEXT& _messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
																 vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
																 vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
																 const vk::DebugUtilsMessageTypeFlagsEXT& _messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
																 vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
																 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) {


				return addDebugCallback(_messageSeverity, _messageType, DebugCallback, nullptr);
			}

			void destroyDebugCallback(const vk::DebugUtilsMessengerEXT& _messenger);

			static std::string SeverityToString(const vk::DebugUtilsMessageSeverityFlagBitsEXT _severity);
			static std::string TypeToString(const vk::DebugUtilsMessageTypeFlagBitsEXT _type);

		private:
			std::shared_ptr<Instance> mInstance;
			std::shared_ptr<Device> mDevice;
			std::vector<vk::DebugUtilsMessengerEXT> mMessengers;

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
																VkDebugUtilsMessageTypeFlagsEXT _messageType,
																const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
																void* _pUserData);
		};
	}
}

#endif // !_MX_VK_DEBUG_H_

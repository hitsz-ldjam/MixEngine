#pragma once
#ifndef MX_VK_SHADER_MODULE_H_
#define MX_VK_SHADER_MODULE_H_

#include "../Device/MxVkDevice.h"

namespace Mix {
	class ShaderSource;

	namespace Vulkan {
		class ShaderModule : public GeneralBase::NoCopyBase {
		public:
			ShaderModule(std::shared_ptr<Device> _device,
				   const uint32_t* _data,
				   const uint32_t& _size,
				   const vk::ShaderStageFlagBits& _stage);

			ShaderModule(std::shared_ptr<Device> _device,
				   const ShaderSource& _source);

			~ShaderModule();

			ShaderModule(ShaderModule&& _other) noexcept { swap(_other); }

			ShaderModule& operator=(ShaderModule&& _other) noexcept { swap(_other); return *this; }

			void swap(ShaderModule& _other) noexcept;

			const vk::ShaderModule& get() const { return mModule; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			const vk::ShaderStageFlagBits& stage() const { return mStage; }

		private:
			std::shared_ptr<Device> mDevice;
			vk::ShaderModule mModule;
			vk::ShaderStageFlagBits mStage;
		};
	}
}
#endif // !MX_VK_SHADER_H_

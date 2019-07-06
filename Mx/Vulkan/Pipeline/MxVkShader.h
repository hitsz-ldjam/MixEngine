#pragma once
#ifndef MX_VK_SHADER_H_
#define MX_VK_SHADER_H_

#include "../Device/MxVkDevice.h"

namespace Mix {
	namespace Resource {
		class ShaderSource;
	}

	namespace Graphics {
		class Shader : public GeneralBase::NoCopyBase {
		public:
			Shader(const std::shared_ptr<Device>& _device,
				   const uint32_t* _data,
				   const uint32_t& _size,
				   const vk::ShaderStageFlagBits& _stage);

			Shader(const std::shared_ptr<Device>& _device,
				   const Resource::ShaderSource& _source);

			~Shader();

			Shader(Shader&& _other) noexcept { swap(_other); }

			Shader& operator=(Shader&& _other) noexcept { swap(_other); return *this; }

			void swap(Shader& _other) noexcept;

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

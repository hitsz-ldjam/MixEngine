#include "MxVkShaderModule.h"
#include <utility>

#include "shaderc/shaderc.hpp"
#include "../../Log/MxLog.h"
#include "../../Resource/Shader/MxShaderSource.h"

namespace Mix {
	namespace Vulkan {
		ShaderModule::ShaderModule(std::shared_ptr<Device> _device,
					   const uint32_t* _data,
					   const uint32_t& _size,
					   const vk::ShaderStageFlagBits& _stage) :mDevice(std::move(_device)) {
			assert(_data && _size > 0);
			vk::ShaderModuleCreateInfo createInfo;
			createInfo.pCode = _data;
			createInfo.codeSize = _size;

			mModule = mDevice->getVkHandle().createShaderModule(createInfo);
			mStage = _stage;
		}

		ShaderModule::ShaderModule(std::shared_ptr<Device> _device, const ShaderSource& _source):mDevice(std::move(_device)) {
			vk::ShaderModuleCreateInfo createInfo;
			createInfo.pCode = _source.getSprvData();
			createInfo.codeSize = _source.getSprvDataSize();
			
			mModule = mDevice->getVkHandle().createShaderModule(createInfo);
			mStage = _source.getStage();
		}

		ShaderModule::~ShaderModule() {
			if (mModule)
				mDevice->getVkHandle().destroyShaderModule(mModule);
		}

		void ShaderModule::swap(ShaderModule& _other) noexcept {
			using std::swap;
			swap(mDevice, _other.mDevice);
			swap(mModule, _other.mModule);
			swap(mStage, _other.mStage);
		}
	}
}

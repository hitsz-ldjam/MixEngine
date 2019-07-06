#include "MxVkShader.h"

#include "../../Exceptions/MxExceptions.hpp"
#include "shaderc/shaderc.hpp"
#include "../../Log/MxLog.h"
#include "../../Resource/Shader/MxShaderSource.h"

namespace Mix {
	namespace Graphics {
		Shader::Shader(const std::shared_ptr<Device>& _device,
					   const uint32_t* _data,
					   const uint32_t& _size,
					   const vk::ShaderStageFlagBits& _stage) :mDevice(_device) {
			assert(_data && _size > 0);
			vk::ShaderModuleCreateInfo createInfo;
			createInfo.pCode = _data;
			createInfo.codeSize = _size;

			mModule = mDevice->get().createShaderModule(createInfo);
			mStage = _stage;
		}

		Shader::Shader(const std::shared_ptr<Device>& _device, const Resource::ShaderSource& _source):mDevice(_device) {
			vk::ShaderModuleCreateInfo createInfo;
			createInfo.pCode = _source.getSprvData();
			createInfo.codeSize = _source.getSprvDataSize();
			
			mModule = mDevice->get().createShaderModule(createInfo);
			mStage = _source.getStage();
		}

		Shader::~Shader() {
			if (mModule)
				mDevice->get().destroyShaderModule(mModule);
		}

		void Shader::swap(Shader& _other) noexcept {
			using std::swap;
			swap(mDevice, _other.mDevice);
			swap(mModule, _other.mModule);
			swap(mStage, _other.mStage);
		}
	}
}

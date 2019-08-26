#pragma once
#ifndef MX_SHADER_SOURCE_H_
#define MX_SHADER_SOURCE_H_

#include "../MxResourceBase.h"
#include <vulkan/vulkan.hpp>

namespace Mix {
	class ShaderSource : public ResourceBase {
		friend class ShaderParser;
	public:
		ShaderSource(const std::vector<uint32_t>& _data,
					 const vk::ShaderStageFlagBits& _stage)
			:mData(_data), mStage(_stage) {
		}

		ShaderSource(std::vector<uint32_t>&& _data,
					 const vk::ShaderStageFlagBits& _stage)
			:mData(std::move(_data)), mStage(_stage) {
		}

		const std::vector<uint32_t>& getSprv() const { return mData; }

		const vk::ShaderStageFlagBits& getStage() const { return mStage; }

		const uint32_t* getSprvData() const { return mData.data(); }

		size_t getSprvDataSize() const { return mData.size() * 4; }

	private:
		std::vector<uint32_t> mData;
		vk::ShaderStageFlagBits mStage;
	};
}


#endif

#pragma once
#ifndef MX_VK_COMMANDBUFF_POOL_H_
#define MX_VK_COMMANDBUFF_POOL_H_

#include "../Device/MxVkDevice.h"
#include "../../Utils/MxGeneralBase.hpp"

namespace Mix {
	namespace Graphics {
		class CommandPool final :public GeneralBase::NoCopyBase {
		public:
			CommandPool(const std::shared_ptr<Device>& _device,
						const vk::QueueFlagBits _queueType,
						const vk::CommandPoolCreateFlags& _flags = vk::CommandPoolCreateFlags());


			CommandPool(CommandPool&& _other) noexcept { swap(_other); }

			CommandPool& operator=(CommandPool&& _other) noexcept { swap(_other); return *this; }

			void swap(CommandPool& _other) noexcept;

			const vk::CommandPool& get() const { return *mCommandPool; }

			const vk::Queue& getQueue() const { return mQueue; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			vk::CommandBuffer allocate(vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);

			std::vector<vk::CommandBuffer> allocate(uint32_t _count,
													vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);

			vk::UniqueCommandBuffer allocateUnique(vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);

			std::vector<vk::UniqueCommandBuffer> allocateUnique(uint32_t _count,
																vk::CommandBufferLevel _level = vk::CommandBufferLevel::ePrimary);

			void free(const vk::CommandBuffer& _cmdBuffer);

			void free(const std::vector<vk::CommandBuffer>& _cmdBuffers);

		private:
			vk::UniqueCommandPool mCommandPool;

			vk::Queue mQueue;
			std::shared_ptr<Device> mDevice;
		};
	}
}

#endif

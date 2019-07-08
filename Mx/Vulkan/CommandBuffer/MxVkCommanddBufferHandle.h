#pragma once
#ifndef MX_VK_CMD_BUFFER_SUBMITTER_H_
#define MX_VK_CMD_BUFFER_SUBMITTER_H_

#include "../../Utils/MxGeneralBase.hpp"
#include <vulkan/vulkan.hpp>
#include <functional>

namespace vk {
	class CommandBuffer;
}

namespace Mix {
	namespace Graphics {
		class CommandPool;

		class CommandBufferHandle :public GeneralBase::NoCopyBase {
		public:
			explicit CommandBufferHandle(const std::shared_ptr<CommandPool>& _commandPool);

			~CommandBufferHandle();

			CommandBufferHandle(CommandBufferHandle&& _other) noexcept { swap(_other); }

			CommandBufferHandle& operator=(CommandBufferHandle&& _other) noexcept { swap(_other); return *this; }

			void swap(CommandBufferHandle& _other) noexcept;

			const vk::CommandBuffer& get() const { return mCommandBuffer; }

			std::shared_ptr<CommandPool> getCommandPool() const { return mCommandPool; }

			void begin(vk::CommandBufferUsageFlagBits _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse);

			void end();

			void addCommand(const std::function<void(const vk::CommandBuffer&)>& _task);

			void addCommand(std::function<void(const vk::CommandBuffer&)>&& _task);

			void addCommand(std::shared_ptr<std::function<void(const vk::CommandBuffer&)>> _task);

			void submit(const std::vector<vk::Semaphore>& _waitSemaphore = {},
						const std::vector<vk::PipelineStageFlags>& _waitStage = {},
						const std::vector<vk::Semaphore>& _signalSemaphore = {}) const;

			vk::Result wait(const uint64_t& _timeOut = std::numeric_limits<uint64_t>::max()) const;

			void clearCommand() { mCommands.clear(); }

		private:

			std::shared_ptr<CommandPool> mCommandPool;
			bool mBegined = false;
			vk::CommandBuffer mCommandBuffer;
			vk::Fence mFence;

			std::vector<std::shared_ptr<std::function<void(const vk::CommandBuffer&)>>> mCommands;
		};
	}
}

#endif

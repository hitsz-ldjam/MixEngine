#include "MxVkCommanddBufferHandle.h"
#include "MxVkCommandPool.h"


namespace Mix {
	namespace Graphics {
		/*CommandBufferSubmitter::CommandBufferSubmitter(const std::shared_ptr<Core>& _core,
													   const vk::QueueFlagBits _queueType
													   uint32_t _bufferCount) :
			mCore(_core),
			mQueue(std::make_shared<vk::Queue>(_core->getQueues().transfer.value())),
			mCommandPool(std::make_shared<vk::CommandPool>(_core->getDevice(), true, true, )),
			mFence(std::move(_core->getSyncObjMgr().createFence())) {
			*mCommandBuffers = mCommandPool->al(vk::CommandBufferLevel::ePrimary, numberCommandBuffers);
		}

		void CommandBufferSubmitter::addObserver(ObserverCmdBufferSubmitter *_observer) {
			mObservers->emplace_back(_observer);
		}

		vk::CommandBuffer CommandBufferSubmitter::createCommandBuffer() {
			if (*mIndex >= mCommandBuffers->size()) {
				auto buffers = mCommandPool->allocate(vk::CommandBufferLevel::ePrimary, 10);

				for (auto &b : buffers)
					mCommandBuffers->emplace_back(b);
			}

			return (*mCommandBuffers)[(*mIndex)++];
		}

		void CommandBufferSubmitter::submit() {
			vk::SubmitInfo info;
			info.setCommandBufferCount(*mIndex).setPCommandBuffers(mCommandBuffers->data());
			mFence->reset();
			mQueue->submit(info, *mFence);
		}

		void CommandBufferSubmitter::wait() {
			*mIndex = 0;
			mFence->wait();
			mFence->reset();
			for (auto &observer : *mObservers)
				observer->notify();
		}*/
		CommandBufferHandle::CommandBufferHandle(const std::shared_ptr<CommandPool>& _commandPool)
			: mCommandPool(_commandPool) {
			mFence = mCommandPool->getDevice()->get().createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			mCommandBuffer = mCommandPool->allocate();
		}

		CommandBufferHandle::~CommandBufferHandle() {
			if (mCommandBuffer) {
				wait();
				mCommandPool->free(mCommandBuffer);
				mCommandPool->getDevice()->get().destroyFence(mFence);
			}
		}

		void CommandBufferHandle::swap(CommandBufferHandle& _other) noexcept {
			using std::swap;
			swap(mCommandPool, _other.mCommandPool);
			swap(mBegined, _other.mBegined);
			swap(mCommandBuffer, _other.mCommandBuffer);
			swap(mFence, _other.mFence);
			swap(mCommands, _other.mCommands);
		}

		void CommandBufferHandle::begin(const vk::CommandBufferUsageFlagBits _usage) {
			if (!mBegined) {
				mCommandBuffer.begin(vk::CommandBufferBeginInfo(_usage));
				mBegined = true;
			}
		}

		void CommandBufferHandle::end() {
			if (mBegined) {
				mCommandBuffer.end();
				mBegined = false;
			}
		}

		void CommandBufferHandle::addCommand(const std::function<void(const vk::CommandBuffer&)>& _task) {
			if (mBegined) {
				mCommands.emplace_back(std::make_shared<std::function<void(const vk::CommandBuffer&)>>(_task));
				(*mCommands.back())(mCommandBuffer);
			}
		}

		void CommandBufferHandle::addCommand(std::function<void(const vk::CommandBuffer&)>&& _task) {
			if (mBegined) {
				mCommands.emplace_back(std::make_shared<std::function<void(const vk::CommandBuffer&)>>(std::move(_task)));
				(*mCommands.back())(mCommandBuffer);
			}
		}

		void CommandBufferHandle::addCommand(std::shared_ptr<std::function<void(const vk::CommandBuffer&)>> _task) {
			if (mBegined) {
				mCommands.emplace_back(std::move(_task));
				(*mCommands.back())(mCommandBuffer);
			}
		}

		void CommandBufferHandle::submit(const std::vector<vk::Semaphore>& _waitSemaphore,
										 const std::vector<vk::PipelineStageFlags>& _waitStage,
										 const std::vector<vk::Semaphore>& _signalSemaphore) const {
			wait();

			vk::SubmitInfo submitInfo;
			submitInfo.waitSemaphoreCount = static_cast<uint32_t>(_waitSemaphore.size());
			submitInfo.pWaitSemaphores = _waitSemaphore.data();
			submitInfo.pWaitDstStageMask = _waitStage.data();
			submitInfo.signalSemaphoreCount = static_cast<uint32_t>(_signalSemaphore.size());
			submitInfo.pSignalSemaphores = _signalSemaphore.data();
			submitInfo.pCommandBuffers = &mCommandBuffer;
			submitInfo.commandBufferCount = 1;
			
			mCommandPool->getDevice()->get().resetFences(mFence);
			mCommandPool->getQueue().submit(submitInfo, mFence);
		}

		vk::Result CommandBufferHandle::wait(const uint64_t& _timeOut) const {
			return mCommandPool->getDevice()->get().waitForFences(mFence, VK_TRUE, _timeOut);
		}
	}
}
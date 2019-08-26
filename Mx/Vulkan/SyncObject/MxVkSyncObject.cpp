#include "MxVkSyncObject.h"

namespace Mix {
	namespace Vulkan {
		Semaphore::Semaphore(const std::shared_ptr<Device>& _device)
			: mDevice(_device) {
			mSemaphore = mDevice->get().createSemaphore(vk::SemaphoreCreateInfo());
		}

		Semaphore::Semaphore(Semaphore&& _other) noexcept {
			swap(_other);
		}

		Semaphore& Semaphore::operator=(Semaphore&& _other) noexcept {
			swap(_other);
			return *this;
		}

		Semaphore::~Semaphore() {
			if (mSemaphore)
				mDevice->get().destroySemaphore(mSemaphore);
		}

		void Semaphore::swap(Semaphore& _other) noexcept {
			mDevice.swap(_other.mDevice);
			std::swap(mSemaphore, _other.mSemaphore);
		}

		Fence::Fence(const std::shared_ptr<Device>& _device, const bool _signaled) :mDevice(_device) {
			mFence = mDevice->get().createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			if (!_signaled)
				reset();
		}

		Fence::Fence(Fence&& _other) noexcept {
			swap(_other);
		}

		Fence& Fence::operator=(Fence&& _other) noexcept {
			swap(_other);
			return *this;
		}

		Fence::~Fence() {
			if (mFence)
				mDevice->get().destroyFence(mFence);
		}

		void Fence::swap(Fence& _other) noexcept {
			mDevice.swap(_other.mDevice);
			std::swap(mFence, _other.mFence);
		}
	}
}

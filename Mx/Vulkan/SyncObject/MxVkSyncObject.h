#pragma once
#ifndef MX_VK_SYNCOBJMGR_H_
#define MX_VK_SYNCOBJMGR_H_

#include "../Device/MxVkDevice.h"

namespace Mix {
	namespace Graphics {
		class Semaphore : GeneralBase::NoCopyBase {
		public:
			Semaphore() = default;

			explicit Semaphore(const std::shared_ptr<Device>& _device);

			Semaphore(Semaphore&& _other) noexcept;

			Semaphore& operator=(Semaphore&& _other) noexcept;

			~Semaphore();

			void swap(Semaphore& _other) noexcept;

			const vk::Semaphore& get() const { return mSemaphore; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			explicit operator bool() const {
				return mSemaphore != nullptr;
			}

		private:
			std::shared_ptr<Device> mDevice;
			vk::Semaphore mSemaphore;
		};

		class Fence : GeneralBase::NoCopyBase {
		public:
			Fence() = default;

			Fence(const std::shared_ptr<Device>& _device, bool _signaled);

			Fence(Fence&& _other) noexcept;

			Fence& operator=(Fence&& _other) noexcept;

			~Fence();

			void swap(Fence& _other) noexcept;

			const vk::Fence& get() const { return mFence; }

			std::shared_ptr<Device> getDevice() const { return mDevice; }

			void reset() const { mDevice->get().resetFences(mFence); }

			vk::Result wait(const uint64_t& _timeout = std::numeric_limits<uint64_t>::max()) const { return mDevice->get().waitForFences(mFence, true, _timeout); }

			bool isSignaled() const { return mDevice->get().getFenceStatus(mFence) == vk::Result::eSuccess; }

			explicit operator bool() const {
				return mFence != nullptr;
			}

		private:
			std::shared_ptr<Device> mDevice;
			vk::Fence mFence;
		};
	}
}
#endif // !MX_VK_SYNCOBJMGR_H_
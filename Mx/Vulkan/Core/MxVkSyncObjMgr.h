#pragma once
#ifndef MX_VK_SYNCOBJMGR_H_
#define MX_VK_SYNCOBJMGR_H_

#include "MxVkDef.h"

namespace Mix {
    namespace Graphics {
        class Fence;
        class Semaphore;

        class SyncObjectMgr{
        public:
            void init(const vk::Device& _device) {
                mDevice = _device;
            }

            ~SyncObjectMgr() {
                destroy();
            }

            Fence createFence(const vk::FenceCreateFlags _flags = vk::FenceCreateFlagBits::eSignaled);

            void destroyFence(const vk::Fence _fence);

            Semaphore createSemaphore();

            void destroySemaphore(const vk::Semaphore _semaphore);

            void destroy();
        private:
            vk::Device mDevice;
            std::vector<vk::Fence> mFences;
            std::vector<vk::Semaphore> mSemaphores;
        };

        class Semaphore {
        public:
            Semaphore() :mMgr(nullptr), mSemaphore(nullptr) {};

            Semaphore(SyncObjectMgr* _mgr, const vk::Semaphore& _sph) :mMgr(_mgr), mSemaphore(_sph) {
            }

            Semaphore(const Semaphore& _other) = delete;

            Semaphore(Semaphore&& _other) noexcept {
                mSemaphore = _other.mSemaphore;
                mMgr = _other.mMgr;

                _other.mSemaphore = nullptr;
                _other.mMgr = nullptr;
            }

            Semaphore& operator=(const Semaphore& _other) = delete;

            Semaphore& operator=(Semaphore&& _other) noexcept {
                mSemaphore = _other.mSemaphore;
                mMgr = _other.mMgr;

                _other.mSemaphore = nullptr;
                _other.mMgr = nullptr;
                return *this;
            }

            void destroy() {
                if (!mMgr)
                    return;

                mMgr->destroySemaphore(mSemaphore);
                mMgr = nullptr;
                mSemaphore = nullptr;
            }

            ~Semaphore() {
                destroy();
            }

            vk::Semaphore& get() {
                return mSemaphore;
            }

        private:
            SyncObjectMgr* mMgr;
            vk::Semaphore mSemaphore;
        };

        class Fence {
        public:
            Fence() :mMgr(nullptr), mFence(nullptr) {};

            Fence(SyncObjectMgr* _mgr, const vk::Fence& _fence) :mMgr(_mgr), mFence(_fence) {
            }

            Fence(const Fence& _other) = delete;

            Fence(Fence&& _other) noexcept {
                mFence = _other.mFence;
                mMgr = _other.mMgr;

                _other.mFence = nullptr;
                _other.mMgr = nullptr;
            }

            Fence& operator=(const Fence& _fence) = delete;

            Fence& operator=(Fence&& _other) noexcept {
                mFence = _other.mFence;
                mMgr = _other.mMgr;

                _other.mFence = nullptr;
                _other.mMgr = nullptr;
                return *this;
            }

            void destroy() {
                if (!mMgr)
                    return;

                mMgr->destroyFence(mFence);
                mMgr = nullptr;
                mFence = nullptr;
            }

            ~Fence() {
                destroy();
            }

            vk::Fence& get() {
                return mFence;
            }

        private:
            SyncObjectMgr* mMgr;
            vk::Fence mFence;
        };
    }
}
#endif // !MX_VK_SYNCOBJMGR_H_
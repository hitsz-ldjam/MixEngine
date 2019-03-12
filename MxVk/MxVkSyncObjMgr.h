#pragma once
#ifndef _MX_VK_SYNCOBJMGR_H_
#define _MX_VK_SYNCOBJMGR_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        class Fence;
        class Semaphore;

        class SyncObjectMgr :public GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            virtual ~SyncObjectMgr() {
                destroy();
            }

            Fence createFence(const vk::FenceCreateFlags flags = vk::FenceCreateFlagBits::eSignaled);

            void destroyFence(const vk::Fence fence);

            Semaphore createSemaphore();

            void destroySemaphore(const vk::Semaphore semaphore);

            void destroy();
        private:
            std::vector<vk::Fence> mFences;
            std::vector<vk::Semaphore> mSemaphores;
        };

        // todo

        class Semaphore :GraphicsBase {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            Semaphore() :mMgr(nullptr), mSemaphore(nullptr) {};

            Semaphore(SyncObjectMgr* mgr, const vk::Semaphore& sph) :mMgr(mgr), mSemaphore(sph) {
            }

            Semaphore(const Semaphore& sph) = delete;

            Semaphore(Semaphore&& sph) noexcept {
                mSemaphore = sph.mSemaphore;
                mMgr = sph.mMgr;

                sph.mSemaphore = nullptr;
                sph.mMgr = nullptr;
            }

            Semaphore& operator=(const Semaphore& sph) = delete;

            Semaphore& operator=(Semaphore&& sph) noexcept {
                mSemaphore = sph.mSemaphore;
                mMgr = sph.mMgr;

                sph.mSemaphore = nullptr;
                sph.mMgr = nullptr;
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

            vk::Semaphore get() {
                return mSemaphore;
            }

        private:
            SyncObjectMgr* mMgr;
            vk::Semaphore mSemaphore;
        };

        class Fence :GraphicsBase {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            Fence() :mMgr(nullptr), mFence(nullptr) {};

            Fence(SyncObjectMgr* mgr, const vk::Fence& sph) :mMgr(mgr), mFence(sph) {
            }

            Fence(const Fence& sph) = delete;

            Fence(Fence&& sph) noexcept {
                mFence = sph.mFence;
                mMgr = sph.mMgr;

                sph.mFence = nullptr;
                sph.mMgr = nullptr;
            }

            Fence& operator=(const Fence& sph) = delete;

            Fence& operator=(Fence&& sph) noexcept {
                mFence = sph.mFence;
                mMgr = sph.mMgr;

                sph.mFence = nullptr;
                sph.mMgr = nullptr;
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

            vk::Fence get() {
                return mFence;
            }

        private:
            SyncObjectMgr* mMgr;
            vk::Fence mFence;
        };
    }
}
#endif // !_MX_VK_SYNCOBJMGR_H_
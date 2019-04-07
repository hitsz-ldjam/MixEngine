#include "MxVkSyncObjMgr.h"

namespace Mix {
    namespace Graphics {
        Fence SyncObjectMgr::createFence(const vk::FenceCreateFlags flags) {
            vk::FenceCreateInfo createInfo = {};
            createInfo.flags = flags;

            mFences.push_back(mDevice.createFence(createInfo));
            return Fence(this, mFences.back());
        }

        void SyncObjectMgr::destroyFence(const vk::Fence fence) {
            auto it = std::find(mFences.begin(), mFences.end(), fence);
            if (it == mFences.end())
                throw SyncObjNotFound();

            mDevice.destroyFence(*it);
            mFences.erase(it);
        }

        Semaphore SyncObjectMgr::createSemaphore() {
            vk::SemaphoreCreateInfo createInfo;
            mSemaphores.push_back(mDevice.createSemaphore(createInfo));
            return Semaphore(this, mSemaphores.back());
        }

        void SyncObjectMgr::destroySemaphore(const vk::Semaphore semaphore) {
            auto it = std::find(mSemaphores.begin(), mSemaphores.end(), semaphore);
            if (it == mSemaphores.end())
                throw SyncObjNotFound();

            mDevice.destroySemaphore(*it);
            mSemaphores.erase(it);
        }

        void SyncObjectMgr::destroy() {
            for (auto& fence : mFences)
                mDevice.destroyFence(fence);
            for (auto& semphore : mSemaphores)
                mDevice.destroySemaphore(semphore);

            mFences.clear();
            mSemaphores.clear();
            mDevice = nullptr;
        }

    }
}

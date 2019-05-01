#include "MxVkSyncObjMgr.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(SyncObjectMgr, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(SyncObjectMgr);

        Fence SyncObjectMgr::createFence(const vk::FenceCreateFlags flags) {
            vk::FenceCreateInfo createInfo = {};
            createInfo.flags = flags;

            mFences.push_back(mCore->device().createFence(createInfo));
            return Fence(this, mFences.back());
        }

        void SyncObjectMgr::destroyFence(const vk::Fence fence) {
            auto it = std::find(mFences.begin(), mFences.end(), fence);
            if (it == mFences.end())
                throw SyncObjNotFound();

            mCore->device().destroyFence(*it);
            mFences.erase(it);
        }

        Semaphore SyncObjectMgr::createSemaphore() {
            vk::SemaphoreCreateInfo createInfo;
            mSemaphores.push_back(mCore->device().createSemaphore(createInfo));
            return Semaphore(this, mSemaphores.back());
        }

        void SyncObjectMgr::destroySemaphore(const vk::Semaphore semaphore) {
            auto it = std::find(mSemaphores.begin(), mSemaphores.end(), semaphore);
            if (it == mSemaphores.end())
                throw SyncObjNotFound();

            mCore->device().destroySemaphore(*it);
            mSemaphores.erase(it);
        }

        void SyncObjectMgr::destroy() {
            for (auto& fence : mFences)
                mCore->device().destroyFence(fence);
            for (auto& semphore : mSemaphores)
                mCore->device().destroySemaphore(semphore);

            mFences.clear();
            mSemaphores.clear();
            mCore = nullptr;
        }

        MX_IMPLEMENT_RTTI_NoCreateFunc(Semaphore, GraphicsBase);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Semaphore);

        MX_IMPLEMENT_RTTI_NoCreateFunc(Fence, GraphicsBase);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Fence);
    }
}

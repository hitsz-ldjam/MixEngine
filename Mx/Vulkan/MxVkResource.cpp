#include "MxVkResource.h"
#include "../Definitions/MxDefinitions.h"
#include "CommandBuffer/MxVkCommandPool.h"


namespace Mix {
    namespace Vulkan {
        Resource::Resource(ResourceManager* _manager, bool _shared) :
            mManager(_manager),
            mQueueFamily(-1),
            mState(_shared ? State::Shadred : State::Normal),
            mNumUsed(0),
            mNumBound(0) {
            memset(mReadQueue, 0, sizeof mReadQueue);
            memset(mWriteQueue, 0, sizeof mWriteQueue);
        }

        Resource::~Resource() {
            MX_ASSERT(mState == State::Destroyed && "Resource should be destroyed before constructed");
        }

        void Resource::notifyBound() {
            MX_ASSERT(mState != State::Destroyed);
            ++mNumBound;
        }

        void Resource::notifyUnbound() {
            --mNumBound;

            bool destroy = mNumBound == 0 && mState == State::Destroyed;

            if (destroy)
                mManager->destroy(this);
        }

        void Resource::notifyUsed(uint32_t _queueIdx, Flags<AccessFlags> _accessFlags) {
        }

        void Resource::ontifyDone(uint32_t _queueIdx, Flags<AccessFlags> _accessFlags) {
        }

        void Resource::destroy() {
            MX_ASSERT(mState != State::Destroyed && "Call destroy() more than once on one resource.");
            mState = State::Destroyed;

            if (mNumBound == 0)
                mManager->destroy(this);
        }


    }
}


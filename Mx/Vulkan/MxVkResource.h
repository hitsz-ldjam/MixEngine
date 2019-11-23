#pragma once
#ifndef MX_VK_RESOURCE_H_
#define MX_VK_RESOURCE_H_
#include <cstdint>
#include "../Utils/MxFlags.h"
#include "../Definitions/MxCommonEnum.h"
#include <bitset>
#include <mutex>

namespace Mix {
    namespace Vulkan {
        class Device;

        enum class AccessFlags {
            None = 0x0000,
            Read = 0x0001,
            Write = 0x0002,
            ReadAndWrite = Read | Write
        };


        class ResourceManager;

        class Resource {
        public:
            explicit Resource(ResourceManager* _manager, bool _shared = false);

            virtual ~Resource();

            void notifyBound();

            void notifyUnbound();

            void notifyUsed(uint32_t _queueIdx, Flags<AccessFlags> _accessFlags);

            void ontifyDone(uint32_t _queueIdx, Flags<AccessFlags> _accessFlags);

            bool isInUse() const { return mNumUsed > 0; }

            bool isBound() const { return mNumBound > 0; }

            uint32_t getUseCount() const { return mNumUsed; }

            uint32_t getBoundCount() const { return mNumBound; }

            bool isExclusive() const { return mState != State::Shadred; }

            void destroy();

        protected:
            enum class State {
                Normal,
                Shadred,
                Destroyed
            };

            static constexpr uint32_t sMaxUniqueQueue = static_cast<const uint32_t>(GPUQueueType::Count);

            ResourceManager* mManager;
            uint32_t mQueueFamily;
            State mState;

            uint8_t mReadQueue[sMaxUniqueQueue];
            uint8_t mWriteQueue[sMaxUniqueQueue];

            uint32_t mNumUsed;
            uint32_t mNumBound;
        };


        class ResourceManager {
        public:
            ResourceManager(std::shared_ptr<Device> _device);

            ~ResourceManager();

            std::shared_ptr<Device> getDevice() const { return mDevice; }

        private:
            friend Resource;

            template<typename _Ty, typename... _Args>
            _Ty* create(_Args&&... _args) {
                static_assert(std::is_constructible_v<_Ty, _Args>, "Cannot construct resource with given arguments");

                _Ty* resource = new _Ty(this, std::forward<_Args>(_args)...);

                return resource;
            }

            void destroy(Resource* _resource) { delete _resource; };


            std::shared_ptr<Device> mDevice;

        };
    }

    MX_ALLOW_FLAGS_FOR_ENUM(Vulkan::AccessFlags);

}

#endif

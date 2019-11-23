#pragma once
#ifndef MX_VK_COMMANDBUFFER_H_
#define MX_VK_COMMANDBUFFER_H_

#include <memory>
#include "MxVkCommanddBufferHandle.h"
#include "../MxVkResource.h"

namespace Mix {
    namespace Vulkan {
        class Device;
        class CommandPool;
        class Queue;
        class Fence;

        class CmdBuffer {
            enum class State {
                Ready,
                Recording,
                InRenderPass,
                RecordingDone,
                Submitted
            };

        public:
            CmdBuffer(std::shared_ptr<CommandPool>& _cmdPool, uint32_t _id, bool _secondary = false);

            ~CmdBuffer() = default;

            uint32_t getId() const { return mId; }

            uint32_t getQueueFamilyIdx() const;

            void begin();

            void end();

            void beginRenderPass();

            void endRenderPass();

            void submit(Queue& _queue);

            vk::CommandBuffer getVkHandle() const { return mCmdBuffer; }

            vk::Fence getFence() const { return mFence; }

            bool isSubmitted() const { return mState == State::Submitted; }

            bool isRecording() const { return mState == State::Recording; }

            bool isRecordingDone() const { return mState == State::RecordingDone; }

            bool isInRenderPass() const { return mState == State::InRenderPass; }

            bool isDone(bool _block);

            void reset();

            void registerResource(Resource* _resource, Flags<AccessFlags> _flags);


        private:
            std::shared_ptr<CommandPool> mCmdPool;
            State mState;
            vk::CommandBuffer mCmdBuffer;
            vk::Fence mFence;
            uint32_t mId;
        };
    }
}

#endif

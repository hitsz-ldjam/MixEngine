#pragma once
#ifndef MX_GPU_QUERY_H_
#define MX_GPU_QUERY_H_
#include <memory>

namespace Mix {
    class CommandBuffer;

    class GPUTimeQuery {
    public:
        virtual ~GPUTimeQuery() = default;

        virtual void begin(const std::shared_ptr<CommandBuffer>& _cmdBuffer = nullptr) = 0;

        virtual void end(const std::shared_ptr<CommandBuffer>& _cmdBuffer = nullptr) = 0;

        virtual bool isReady() const = 0;

        virtual float getTime() = 0;

        static std::shared_ptr<GPUTimeQuery> Create();

    protected:

        bool isActive() const { return mIsActive; }

        void setActive(bool _active) { mIsActive = _active; }

        bool mIsActive = true;
    };


    class GPUEventQuery {
    public:
        virtual ~GPUEventQuery() = default;

        virtual void begin(const std::shared_ptr<CommandBuffer>& _cmdBuffer = nullptr) = 0;

        virtual bool isReady() const = 0;

        static std::shared_ptr<GPUEventQuery> Create();

    private:

        bool isActive() const { return mIsActive; }

        void setActive(bool _active) { mIsActive = _active; }

        bool mIsActive = true;

    };
}

#endif

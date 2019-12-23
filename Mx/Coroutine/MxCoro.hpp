#pragma once

#ifndef MX_CORO_HPP_
#define MX_CORO_HPP_

#include "MxYieldHandle.hpp"

namespace Mix::Coroutine {
    class Coro final : public YieldInstruction {
        friend class CoroMgr;

    public:
        bool isValid() const noexcept { return bool(coro); }

    private:
        std::unique_ptr<YieldHandle> handle;
        CoroType::pull_type coro;

        template<typename _Fn, typename ... _Args>
        Coro(_Fn&& _fn, _Args&& ... _args)
            : YieldInstruction(YieldInstructionType::Coro),
              handle(nullptr),
              // default by-ref capture suffices
              coro([&](CoroType::push_type& _yield) {
                  handle.reset(new YieldHandle(_yield));
                  _fn(*handle, std::forward<_Args>(_args)...);
              }) {}

        auto getInstruction() noexcept { return coro.get(); }

        void resume() { coro(); }
    };
}

#endif

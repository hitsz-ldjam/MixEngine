#pragma once

#ifndef MX_YIELD_HANDLE_HPP_
#define MX_YIELD_HANDLE_HPP_

#include "MxCoroType.hpp"

namespace Mix::Coroutine {
    class YieldHandle final {
    public:
        YieldHandle(CoroType::push_type& _yield) : yield(_yield) {}
        void operator()(nullptr_t) { yield(std::make_shared<YieldInstruction>(nullptr)); }
        void operator()(YieldInstruction* _y) { yield(std::shared_ptr<YieldInstruction>(_y)); }
        void operator()(const std::shared_ptr<YieldInstruction>& _y) { yield(_y); }
        void operator()(std::shared_ptr<YieldInstruction>&& _y) { yield(std::move(_y)); }

    private:
        CoroType::push_type& yield;
    };
}

#endif

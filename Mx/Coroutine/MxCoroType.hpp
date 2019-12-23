#pragma once

#ifndef MX_CORO_TYPE_HPP_
#define MX_CORO_TYPE_HPP_

#include "MxYieldInstruction.h"
#include <boost/coroutine2/coroutine.hpp>

namespace Mix::Coroutine {
    using CoroType = boost::coroutines2::coroutine<std::shared_ptr<YieldInstruction>>;
}

#endif

#pragma once

#ifndef MX_COROUTINE_HPP_
#define MX_COROUTINE_HPP_

#include "MxCoroMgr.h"

#define MX_YIELD_HANDLE Mix::Coroutine::YieldHandle& yield_return

namespace Mix::Coroutine {
    /**
     *  @brief Starts a coroutine. The first argument of a coroutine should be@code MX_YIELD_HANDLE@endcode. 
     *
     *  @param _fn Any callable object whose first argument is@code MX_YIELD_HANDLE@endcode.
     *  @param _args Other parameters to be passed to _fn.
     *
     *  @note
     *  1) The callable object and its arguments are forwarded into an internal stack and will be released once
     *  coroutine ends. Lifetime of the source of pointer/reference arguments should be managed by the caller.\n
     *  2) DO NOT call coroutines recursively or form any loop of call.\n
     *  3) Coroutines should not throw exceptions since exception handling has not yet been implemented.\n
     *
     *  @example
     *  @code
     *  void bar_free(MX_YIELD_HANDLE, int i);
     *
     *  class Foo {
     *      void bar_mem(MX_YIELD_HANDLE, int i);
     *
     *      void start() {
     *          // behaves like a free function
     *          Coroutine::startCoroutine([](MX_YIELD_HANDLE, int a) {}, int(some_val)));
     *
     *          // behaves like a member function
     *          Coroutine::startCoroutine([this](MX_YIELD_HANDLE, int a) {}, int(some_val)));
     *
     *          // prefer lambdas to free functions
     *          Coroutine::startCoroutine(bar_free, int(some_val)));
     *
     *          // prefer lambdas to std::bind
     *          Coroutine::startCoroutine(std::bind(&Foo::bar_mem, this, std::placeholders::_1, int(some_val)));
     *          Coroutine::startCoroutine(std::bind(&Foo::bar_mem, this, std::placeholders::_1, std::placeholders::_2), int(some_val));
     *      }
     *  }
     *  @endcode
     */
    template<typename _Fn, typename ... _Args>
    std::shared_ptr<Coro> startCoroutine(_Fn&& _fn, _Args&&... _args) {
        return CoroMgr::Get()->startCoroutine(std::forward<_Fn>(_fn), std::forward<_Args>(_args)...);
    }
}

#endif

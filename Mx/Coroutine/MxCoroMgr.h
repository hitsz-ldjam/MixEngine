#pragma once

#ifndef MX_CORO_MGR_H_
#define MX_CORO_MGR_H_

#include "MxCoro.hpp"
#include "../Engine/MxModuleBase.h"
#include <queue>

namespace Mix::Coroutine {
    class CoroMgr final : public ModuleBase {
    public:
        static CoroMgr* Get();

        // should be called between fixedUpdate() & update()
        void update();

        template<typename _Fn, typename ... _Args>
        std::shared_ptr<Coro> startCoroutine(_Fn&& _fn, _Args&&... _args) {
            // make_shared cannot access the ctor
            std::shared_ptr<Coro> coro(new Coro(std::forward<_Fn>(_fn), std::forward<_Args>(_args)...));
            if(coro->isValid())
                pushCoroAndIns(coro);
            return coro;
        }

        // todo
        //void stopCoroutine(std::shared_ptr<Coro>& _coro);

        void stopAllCoroutines();

    private:
        std::queue<std::shared_ptr<Coro>> ySrc;
        std::queue<std::shared_ptr<YieldInstruction>> yIns;

        // Validity should be checked by caller
        void pushCoroAndIns(std::shared_ptr<Coro>& _coro);
    };
}

#endif

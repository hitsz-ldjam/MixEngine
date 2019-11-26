#include "MxCoroMgr.h"
#include "../Time/MxTime.h"
#include "../../MixEngine.h"

namespace Mix::Coroutine {
    CoroMgr* CoroMgr::Get() {
        return MixEngine::Instance().getModule<CoroMgr>();
    }

    void CoroMgr::update() {
        if(ySrc.empty())
            return;

        ySrc.emplace(nullptr); // nullptr marks the end of a frame

        while(auto& currCoro = ySrc.front()) {
            auto& currIns = yIns.front();
            bool resume = false;

            switch(currIns->getType()) {
                case YieldInstructionType::WaitForSeconds:
                {
                    // no need to use static_pointer_cast
                    auto wfs = static_cast<WaitForSeconds*>(currIns.get());
                    resume = wfs->s <= 0;
                    wfs->s -= Time::DeltaTime(); // todo: check first or update first?
                    break;
                }

                case YieldInstructionType::Coro:
                {
                    auto coro = static_cast<Coro*>(currIns.get());
                    resume = !coro->isValid();
                    break;
                }

                case YieldInstructionType::YieldInstruction:
                {
                    resume = true;
                    break;
                }

                case YieldInstructionType::WaitForFrames:
                {
                    auto wff = static_cast<WaitForFrames*>(currIns.get());
                    resume = wff->f == 0;
                    --wff->f;
                    break;
                }

                case YieldInstructionType::Unkown:
                    break;

                default:
                    break;
            }

            if(resume) {
                // coro in ySrc are all valid
                currCoro->resume();
                if(currCoro->isValid())
                    pushCoroAndIns(currCoro);
            }
            else {
                ySrc.emplace(currCoro);
                yIns.emplace(currIns);
            }

            yIns.pop();
            ySrc.pop();
        }

        ySrc.pop(); // pop the ending nullptr
    }

    void CoroMgr::stopAllCoroutines() {
        {
            decltype(yIns) empty;
            yIns.swap(empty);
        }
        {
            decltype(ySrc) empty;
            ySrc.swap(empty);
        }
    }

    void CoroMgr::pushCoroAndIns(std::shared_ptr<Coro>& _coro) {
        ySrc.emplace(_coro);
        yIns.emplace(_coro->getInstruction());
    }
}

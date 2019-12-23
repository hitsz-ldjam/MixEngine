#pragma once

#ifndef MX_YIELD_INSTRUCTION_H_
#define MX_YIELD_INSTRUCTION_H_

#include <memory>

namespace Mix::Coroutine {
    class Coro;

    enum class YieldInstructionType {
        YieldInstruction,
        Coro,
        WaitForSeconds,
        WaitForFrames,
        /*WaitForFixUpdate,*/
        /*WaitWhile,*/
        /*WaitUntil,*/
        Unkown,
    };

    class YieldInstruction {
        friend class CoroMgr;

    public:
        YieldInstruction(nullptr_t);
        YieldInstruction(const YieldInstruction&) = delete;
        virtual ~YieldInstruction();
        auto getType() const noexcept { return type; }

    protected:
        YieldInstructionType type;
        YieldInstruction(const YieldInstructionType _type);
    };

    class WaitForSeconds final : public YieldInstruction {
        friend class CoroMgr;

    public:
        WaitForSeconds(const float _s) : YieldInstruction(YieldInstructionType::WaitForSeconds),
                                         s(_s) {}

    private:
        float s;
    };

    class WaitForFrames final : public YieldInstruction {
        friend class CoroMgr;

    public:
        WaitForFrames(const unsigned _f) : YieldInstruction(YieldInstructionType::WaitForFrames),
                                           f(_f) {}

    private:
        unsigned f;
    };
}

#endif

#include "MxYieldInstruction.h"
#include "MxCoro.hpp"

namespace Mix::Coroutine {
    YieldInstruction::YieldInstruction(nullptr_t)
        : YieldInstruction(YieldInstructionType::YieldInstruction) {}

    YieldInstruction::~YieldInstruction() = default;

    YieldInstruction::YieldInstruction(const YieldInstructionType _type) : type(_type) {}
}

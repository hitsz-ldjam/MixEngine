#pragma once

#ifndef MX_MODULE_BASE_H_
#define MX_MODULE_BASE_H_

#include "../Utils/MxGeneralBase.hpp"

namespace Mix {
    class ModuleBase : GeneralBase::NoCopyBase {
    public:
        virtual void load() {}

        virtual void init() {}

        virtual ~ModuleBase() = default;

    protected:
        ModuleBase() = default;
    };
}

#endif

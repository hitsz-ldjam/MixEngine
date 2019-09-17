#pragma once
#ifndef MX_SYSTEM_INFO_
#define MX_SYSTEM_INFO_

#include "../Utils/MxGeneralBase.hpp"
#include "MxVersion.h"

namespace Mix {
    class SystemInfo :public GeneralBase::StaticBase {
    public:
        static const std::string EngineName;

        static const Version EngineVersion;
    };
}

#endif

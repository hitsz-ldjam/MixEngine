#pragma once
#ifndef MX_MODULE_BASE_H_
#define MX_MODULE_BASE_H_

#include "../Definitions/MxGeneralBase.hpp"

namespace Mix {
	class ModuleBase : GeneralBase::NoCopyBase {
	public:
		virtual ~ModuleBase() = default;
	};
}

#endif

#pragma once

#ifndef MX_MODULE_BASE_H_
#define MX_MODULE_BASE_H_

#include "../Utils/MxGeneralBase.hpp"

namespace Mix {
	class ModuleBase : GeneralBase::NoCopyBase {
	public:
		virtual void load() = 0 {};

		virtual void init() = 0 {};

		virtual ~ModuleBase() = default;
	};
}

#endif

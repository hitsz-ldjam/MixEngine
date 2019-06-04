#pragma once
#ifndef MX_RESOURCEBASE_H_
#define MX_RESOURCEBASE_H_

#include "../Definitions/MxGeneralBase.hpp"

namespace Mix {
	namespace Resource {
		enum class ResourceType {
			UNKNOWN,
			GLTF_BIN,
			GLTF_ASCII,
			PNG,
			JPG
		};

		class ResourceBase {
		public:
			ResourceBase() = default;
			virtual ~ResourceBase() = default;
		};

	}
}

#endif

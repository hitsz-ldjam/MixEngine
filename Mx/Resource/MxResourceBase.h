#pragma once
#ifndef MX_RESOURCEBASE_H_
#define MX_RESOURCEBASE_H_

namespace Mix {
	enum class ResourceType {
		UNKNOWN,
		// GLTF Model type
		GLTF_BIN,
		GLTF_ASCII,
		// KTX and DDS texture type
		KTX,
		DDS,
		// GLSL and SPIR-V shader type
		GLSL_VERT,
		GLSL_FRAG,
		GLSL_GEOMETRY,
		GLSL_TESS_CTRL,
		GLSL_TESS_EVLT,
		GLSL_COMPUTE,

		PNG,
		JPG,

		SPIRV_VERT,
		SPIRV_FRAG,
		SPIRV_GEOMETRY,
		SPIRV_TESS_CTRL,
		SPIRV_TESS_EVLT,
		SPIRV_COMPUTE
	};


	class ResourceBase {
	public:
		ResourceBase() = default;
		virtual ~ResourceBase() = default;
	};


	template<typename _Target>
	auto GetAdditionalParam();

}

#endif

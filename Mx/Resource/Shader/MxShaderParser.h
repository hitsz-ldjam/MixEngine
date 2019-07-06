#pragma once
#ifndef MX_SHADER_PARSER_H_
#define MX_SHADER_PARSER_H_

#include <shaderc/shaderc.hpp>
#include "../MxResourceParserBase.hpp"
#include "MxShaderSource.h"

#define RESOURCE_GLSL_VERT_EXT "vert"
#define RESOURCE_GLSL_FRAG_EXT "frag"
#define RESOURCE_GLSL_GEOM_EXT "geom"
#define RESOURCE_GLSL_TESC_EXT "tesc"
#define RESOURCE_GLSL_TESE_EXT "tese"
#define RESOURCE_GLSL_COMP_EXT "comp"

#define RESOURCE_SPRV_VERT_EXT "vertspv"
#define RESOURCE_SPRV_FRAG_EXT "fragspv"
#define RESOURCE_SPRV_GEOM_EXT "geomspv"
#define RESOURCE_SPRV_TESC_EXT "tescspv"
#define RESOURCE_SPRV_TESE_EXT "tesespv"
#define RESOURCE_SPRV_COMP_EXT "compspv"



namespace Mix {
	namespace Resource {
		class ShaderParser : public ResourceParserBase {
		public:
			ShaderParser() {
				mSupportedTypes.insert(ResourceType::GLSL_VERT);
				mSupportedTypes.insert(ResourceType::GLSL_FRAG);
				mSupportedTypes.insert(ResourceType::GLSL_GEOMETRY);
				mSupportedTypes.insert(ResourceType::GLSL_TESS_CTRL);
				mSupportedTypes.insert(ResourceType::GLSL_TESS_EVLT);
				mSupportedTypes.insert(ResourceType::GLSL_COMPUTE);

				mSupportedTypes.insert(ResourceType::SPIRV_VERT);
				mSupportedTypes.insert(ResourceType::SPIRV_FRAG);
				mSupportedTypes.insert(ResourceType::SPIRV_GEOMETRY);
				mSupportedTypes.insert(ResourceType::SPIRV_TESS_CTRL);
				mSupportedTypes.insert(ResourceType::SPIRV_TESS_EVLT);
				mSupportedTypes.insert(ResourceType::SPIRV_COMPUTE);

				mSupportedExts.insert(RESOURCE_GLSL_VERT_EXT);
				mSupportedExts.insert(RESOURCE_GLSL_FRAG_EXT);
				mSupportedExts.insert(RESOURCE_GLSL_GEOM_EXT);
				mSupportedExts.insert(RESOURCE_GLSL_TESC_EXT);
				mSupportedExts.insert(RESOURCE_GLSL_TESE_EXT);
				mSupportedExts.insert(RESOURCE_GLSL_COMP_EXT);

				mSupportedExts.insert(RESOURCE_SPRV_VERT_EXT);
				mSupportedExts.insert(RESOURCE_SPRV_FRAG_EXT);
				mSupportedExts.insert(RESOURCE_SPRV_GEOM_EXT);
				mSupportedExts.insert(RESOURCE_SPRV_TESC_EXT);
				mSupportedExts.insert(RESOURCE_SPRV_TESE_EXT);
				mSupportedExts.insert(RESOURCE_SPRV_COMP_EXT);
			}

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type) override;

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext) override;

		private:
			shaderc::Compiler mCompiler;

			std::vector<uint32_t> compileGlslToSpv(
				const char* _data,
				const uint32_t _size,
				const shaderc_shader_kind _kind, const std::string& _name) const;

			bool static IsGlsl(const ResourceType _type);
		};
	}
}

#endif

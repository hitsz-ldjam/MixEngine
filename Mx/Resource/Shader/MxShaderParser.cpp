#include "MxShaderParser.h"
#include "../../Log/MxLog.h"
#include "../../Math/MxMath.h"
#include "MxShaderParser.h"

namespace Mix {
	std::shared_ptr<ResourceBase> ShaderParser::load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) {
		std::ifstream inFile(_path, std::ios_base::binary);
		auto open = inFile.is_open();
		const size_t size = std::filesystem::file_size(_path);

		std::vector<uint32_t> fileData(Math::Align(size, 4) / 4);
		inFile.read(reinterpret_cast<char*>(fileData.data()), size);
		inFile.close();

		if (IsGlsl(_type)) {
			shaderc_shader_kind kind;
			vk::ShaderStageFlagBits stage;
			switch (_type) {
			case ResourceType::GLSL_VERT: kind = shaderc_vertex_shader, stage = vk::ShaderStageFlagBits::eVertex;
				break;
			case ResourceType::GLSL_FRAG: kind = shaderc_fragment_shader, stage = vk::ShaderStageFlagBits::eFragment;
				break;
			case ResourceType::GLSL_GEOMETRY:
				kind = shaderc_geometry_shader, stage = vk::ShaderStageFlagBits::eGeometry;
				break;
			case ResourceType::GLSL_TESS_CTRL:
				kind = shaderc_tess_control_shader, stage = vk::ShaderStageFlagBits::eTessellationControl;
				break;
			case ResourceType::GLSL_TESS_EVLT:
				kind = shaderc_tess_evaluation_shader, stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
				break;
			case ResourceType::GLSL_COMPUTE:
				kind = shaderc_compute_shader, stage = vk::ShaderStageFlagBits::eCompute;
				break;
			default:
				Log::Error("Unknown shader type");
				return nullptr;
			}

			auto spvCode = compileGlslToSpv(reinterpret_cast<const char*>(fileData.data()),
											size,
											kind,
											_path.filename().string());

			return std::make_shared<ShaderSource>(std::move(spvCode), stage);
		}
		else {
			vk::ShaderStageFlagBits stage;
			switch (_type) {
			case ResourceType::SPIRV_VERT: stage = vk::ShaderStageFlagBits::eVertex;
				break;
			case ResourceType::SPIRV_FRAG: stage = vk::ShaderStageFlagBits::eFragment;
				break;
			case ResourceType::SPIRV_GEOMETRY: stage = vk::ShaderStageFlagBits::eGeometry;
				break;
			case ResourceType::SPIRV_TESS_CTRL: stage = vk::ShaderStageFlagBits::eTessellationControl;
				break;
			case ResourceType::SPIRV_TESS_EVLT: stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
				break;
			case ResourceType::SPIRV_COMPUTE: stage = vk::ShaderStageFlagBits::eCompute;
				break;
			default:
				Log::Error("Unknown shader type");
				return nullptr;
			}

			return std::make_shared<ShaderSource>(std::move(fileData), stage);
		}
	}

	std::shared_ptr<ResourceBase> ShaderParser::load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) {
		ResourceType type;
		if (_ext == RESOURCE_GLSL_VERT_EXT) type = ResourceType::GLSL_VERT;
		else if (_ext == RESOURCE_GLSL_FRAG_EXT) type = ResourceType::GLSL_FRAG;
		else if (_ext == RESOURCE_GLSL_GEOM_EXT) type = ResourceType::GLSL_GEOMETRY;
		else if (_ext == RESOURCE_GLSL_TESC_EXT) type = ResourceType::GLSL_TESS_CTRL;
		else if (_ext == RESOURCE_GLSL_TESE_EXT) type = ResourceType::GLSL_TESS_EVLT;
		else if (_ext == RESOURCE_GLSL_COMP_EXT) type = ResourceType::GLSL_COMPUTE;

		else if (_ext == RESOURCE_SPRV_VERT_EXT) type = ResourceType::SPIRV_VERT;
		else if (_ext == RESOURCE_SPRV_FRAG_EXT) type = ResourceType::SPIRV_FRAG;
		else if (_ext == RESOURCE_SPRV_GEOM_EXT) type = ResourceType::SPIRV_GEOMETRY;
		else if (_ext == RESOURCE_SPRV_TESC_EXT) type = ResourceType::SPIRV_TESS_CTRL;
		else if (_ext == RESOURCE_SPRV_TESE_EXT) type = ResourceType::SPIRV_TESS_EVLT;
		else if (_ext == RESOURCE_SPRV_COMP_EXT) type = ResourceType::SPIRV_COMPUTE;

		else {
			Log::Error("Unknown shader file extension");
			return nullptr;
		}

		return load(_path, type, _additionalParam);
	}

	std::vector<uint32_t> ShaderParser::compileGlslToSpv(const char* _data,
														 const size_t _size,
														 const shaderc_shader_kind _kind,
														 const std::string& _name) const {
		shaderc::CompileOptions option;
		option.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
		option.SetSourceLanguage(shaderc_source_language_glsl);

		auto compileResult = mCompiler.CompileGlslToSpv(_data, _size, _kind, _name.c_str());
		if (compileResult.GetCompilationStatus() != shaderc_compilation_status_success) {
			Log::Error(compileResult.GetErrorMessage());
			return {};
		}

		return std::vector<uint32_t>(compileResult.begin(), compileResult.end());
	}

	bool ShaderParser::IsGlsl(const ResourceType _type) {
		return _type == ResourceType::GLSL_VERT ||
			_type == ResourceType::GLSL_FRAG ||
			_type == ResourceType::GLSL_GEOMETRY ||
			_type == ResourceType::GLSL_TESS_CTRL ||
			_type == ResourceType::GLSL_TESS_EVLT ||
			_type == ResourceType::GLSL_COMPUTE;
	}
}

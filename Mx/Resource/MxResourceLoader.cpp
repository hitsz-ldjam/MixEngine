#include "MxResourceLoader.h"

#include "../Utils/MxGuid.h"
#include "../Log/MxLog.h"
#include "../Utils/MxUtils.h"
#include "Model/Gltf/MxGltfParser.h"
#include "Texture/GLI/MxGliParser.h"
#include "Shader/MxShaderParser.h"

namespace Mix {
	namespace Resource {
		void ResourceLoader::init() {
			mLoaderRegister = std::make_shared<ParserRegister>();

			mLoaderRegister->registerParser(std::make_shared<GltfParser>());
			mLoaderRegister->registerParser(std::make_shared<GliParser>());
			mLoaderRegister->registerParser(std::make_shared<ShaderParser>());
		}

		std::shared_ptr<ResourceBase> ResourceLoader::load(const std::string& _file) const {
			// get lower case absolute file path
			const auto path = Utils::GetGenericPath(_file);

			if (!std::filesystem::is_regular_file(path)) {
				Debug::Log::Warning("%s: Failed to load [%s]", __FUNCTION__, _file);
				return nullptr;
			}

			// no extension
			if (path.extension().empty()) {
				Debug::Log::Warning("%s: [ %s ] has no extension.", __FUNCTION__, _file.c_str());
				return nullptr;
			}

			// chech if file has already been loaded
			const auto extension = path.extension().string().substr(1);
			const auto loader = mLoaderRegister->findLoaderByExt(extension);
			if (loader == nullptr) {
				Debug::Log::Warning("%s: No loader support extenxion [ %s ].", __FUNCTION__, extension.c_str());
				return nullptr;
			}

			return loader->load(path, extension);
		}


		std::shared_ptr<ResourceBase> ResourceLoader::load(const std::string& _file, const ResourceType _type) {
			// get lower case absolute file path
			const auto path = Utils::GetGenericPath(_file);

			if (!std::filesystem::is_regular_file(path)) {
				Debug::Log::Warning("%s: Failed to load [%s]", __FUNCTION__, _file);
				return nullptr;
			}

			const auto loader = mLoaderRegister->findLoaderByType(_type);
			if (loader == nullptr) {
				Debug::Log::Warning("%s: No loader support extenxion [ %d ].", __FUNCTION__, _type);
				return nullptr;
			}

			return loader->load(path, _type);
		}
	}
}

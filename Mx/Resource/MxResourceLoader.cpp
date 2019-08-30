#include "MxResourceLoader.h"

#include "../Utils/MxGuid.h"
#include "../Log/MxLog.h"
#include "../Utils/MxUtils.h"
#include "Texture/GLI/MxGliParser.h"
#include "Shader/MxShaderParser.h"
#include "Texture/MxImageParser.h"
#include "Model/Gltf/MxGltf.h"
#include "../../MixEngine.h"

namespace Mix {
	ResourceLoader* ResourceLoader::Get() {
		return MixEngine::Instance().getModule<ResourceLoader>();
	}

	void ResourceLoader::awake() {
		mLoaderRegister = std::make_shared<ParserRegister>();

		mLoaderRegister->registerParser(std::make_shared<Gltf>());
		mLoaderRegister->registerParser(std::make_shared<GliParser>());
		mLoaderRegister->registerParser(std::make_shared<ShaderParser>());
		mLoaderRegister->registerParser(std::make_shared<ImageParser>());
	}

	std::shared_ptr<ResourceBase> ResourceLoader::load(const std::string& _file, void* _additionalParam) const {
		// get lower case absolute file path
		const auto path = Utils::GetGenericPath(_file);

		if (!std::filesystem::is_regular_file(path)) {
			Log::Warning("%s: Failed to load [%s]", __FUNCTION__, _file.c_str());
			return nullptr;
		}

		// no extension
		if (path.extension().empty()) {
			Log::Warning("%s: [ %s ] has no extension.", __FUNCTION__, _file.c_str());
			return nullptr;
		}

		// chech if file has already been loaded
		const auto extension = path.extension().string().substr(1);
		const auto loader = mLoaderRegister->findLoaderByExt(extension);
		if (loader == nullptr) {
			Log::Warning("%s: No loader support extenxion [ %s ].", __FUNCTION__, extension.c_str());
			return nullptr;
		}

		return loader->load(path, extension, _additionalParam);
	}


	std::shared_ptr<ResourceBase> ResourceLoader::load(const std::string& _file, const ResourceType _type, void* _additionalParam) {
		// get lower case absolute file path
		const auto path = Utils::GetGenericPath(_file);

		if (!std::filesystem::is_regular_file(path)) {
			Log::Warning("%s: Failed to load [%s]", __FUNCTION__, _file);
			return nullptr;
		}

		const auto loader = mLoaderRegister->findLoaderByType(_type);
		if (loader == nullptr) {
			Log::Warning("%s: No loader support extenxion [ %d ].", __FUNCTION__, ToString(_type));
			return nullptr;
		}

		return loader->load(path, _type, _additionalParam);
	}

}

#pragma once
#pragma once
#ifndef MX_IMGAE_PARSER_H_
#define MX_IMAGE_PARSER_H_
#include "MxTextureParserBase.hpp"

namespace Mix {
	class ImageParser :public TextureParserBase {
	public:
		ImageParser() {
			mSupportedTypes.insert(ResourceType::PNG);
			mSupportedTypes.insert(ResourceType::JPG);

			mSupportedExts.insert("png");
			mSupportedExts.insert("jpg");
		}

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) override;

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) override;
	};
}

#endif

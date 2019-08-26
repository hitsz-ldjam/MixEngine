#pragma once
#ifndef MX_GLI_PARSER_H_
#define MX_GLI_PARSER_H_
#include "../MxTextureParserBase.hpp"

namespace Mix {
	class GliParser :public TextureParserBase {
	public:
		GliParser() {
			mSupportedTypes.insert(ResourceType::KTX);
			mSupportedTypes.insert(ResourceType::DDS);

			mSupportedExts.insert("ktx");
			mSupportedExts.insert("dds");
		}

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) override;

		std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) override;

	private:
		static std::shared_ptr<ResourceBase> GliToTexture(const gli::texture& _texture);
		static std::shared_ptr<ResourceBase> ToTexture2D(const gli::texture& _texture);
		static std::shared_ptr<ResourceBase> ToCubeMap(const gli::texture& _texture);

		static TextureFormat GliFormatToTextureFormat(const gli::format _format);

	};
}

#endif

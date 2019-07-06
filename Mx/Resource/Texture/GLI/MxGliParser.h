#pragma once
#ifndef MX_GLI_PARSER_H_
#define MX_GLI_PARSER_H_
#include "../MxTextureParserBase.hpp"
#include "../../../Vulkan/MxVkGraphics.h"

namespace Mix {
	namespace Resource {
		class GliParser :public TextureParserBase {
		public:
			GliParser() {
				mSupportedTypes.insert(ResourceType::KTX);
				mSupportedTypes.insert(ResourceType::DDS);

				mSupportedExts.insert("ktx");
				mSupportedExts.insert("dds");
			}

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const ResourceType _type) override;

			std::shared_ptr<ResourceBase> load(const std::filesystem::path& _path, const std::string& _ext) override;

		private:
			std::shared_ptr<ResourceBase> gliToTexture2D(const gli::texture& _texture) const;

			static vk::Format GliToVulkanFormat(const gli::format _format);

		};
	}
}

#endif

#include "MxGliParser.h"
#include <gli/gli.hpp>
#include "../../../../MixEngine.h"

namespace Mix {
	namespace Resource {

		std::shared_ptr<ResourceBase> GliParser::load(const std::filesystem::path& _path, const ResourceType _type) {
			gli::texture texture;

			if (_type == ResourceType::KTX)
				texture = gli::load_ktx(_path.generic_string());
			else if (_type == ResourceType::DDS)
				texture = gli::load_dds(_path.generic_string());

			return gliToTexture2D(texture);
		}

		std::shared_ptr<ResourceBase> GliParser::load(const std::filesystem::path& _path, const std::string& _ext) {
			gli::texture texture;

			if (_ext == "ktx")
				texture = gli::load_ktx(_path.generic_string());
			else if (_ext == "dds")
				texture = gli::load_dds(_path.generic_string());

			return gliToTexture2D(texture);
		}

		std::shared_ptr<ResourceBase> GliParser::gliToTexture2D(const gli::texture& _texture) const {
			auto vulkan = MixEngine::Instance().getModule<Graphics::Vulkan>();
			return std::make_shared<Texture2D>(_texture.extent().x,
											   _texture.extent().y,
											   GliToVulkanFormat(_texture.format()),
											   _texture.size(),
											   reinterpret_cast<const char*>(_texture.data()));
		}

		vk::Format GliParser::GliToVulkanFormat(const gli::format _format) {
			vk::Format result;
			switch (_format) {
			case gli::FORMAT_RGBA8_UNORM_PACK8:
			case gli::FORMAT_RGBA8_UNORM_PACK32:
				result = vk::Format::eR8G8B8A8Unorm;
				break;
			case gli::FORMAT_BGRA8_UNORM_PACK8:
				result = vk::Format::eB8G8R8A8Unorm;
				break;
			default:
				result = vk::Format::eUndefined;
				break;
			}
			return result;
		}


	}
}

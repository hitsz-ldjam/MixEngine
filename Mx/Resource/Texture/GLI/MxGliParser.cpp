#include "MxGliParser.h"
#include <gli/gli.hpp>
#include "../../../../MixEngine.h"
#include "../../../Graphics/Texture/MxTexture.h"

namespace Mix {

	std::shared_ptr<ResourceBase> GliParser::load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) {
		gli::texture texture;

		if (_type == ResourceType::KTX)
			texture = gli::load_ktx(_path.generic_string());
		else if (_type == ResourceType::DDS)
			texture = gli::load_dds(_path.generic_string());

		return GliToTexture(texture);
	}

	std::shared_ptr<ResourceBase> GliParser::load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) {
		gli::texture texture;

		if (_ext == "ktx")
			texture = gli::load_ktx(_path.generic_string());
		else if (_ext == "dds")
			texture = gli::load_dds(_path.generic_string());

		return GliToTexture(texture);
	}

	std::shared_ptr<ResourceBase> GliParser::GliToTexture(const gli::texture& _texture) {
		std::shared_ptr<ResourceBase> result;
		switch (_texture.target()) {
		case gli::TARGET_2D: return ToTexture2D(_texture);
		case gli::TARGET_CUBE: return ToCubeMap(_texture);
		default: return nullptr;
		}
	}

	std::shared_ptr<ResourceBase> GliParser::ToTexture2D(const gli::texture& _texture) {
		auto result = std::make_shared<Texture2D>(_texture.extent().x, _texture.extent().y, GliFormatToTextureFormat(_texture.format()), _texture.levels());

		const char* data = reinterpret_cast<const char*>(_texture.data());
		gli::texture::size_type offset = 0;
		auto mipLevel = _texture.levels();

		for (uint32_t mip = 0; mip < mipLevel; ++mip) {
			result->setPixels(data + offset, _texture.size(mip), mip);
			offset += _texture.size(mip);
		}
		result->apply(false);

		return result;
	}

	std::shared_ptr<ResourceBase> GliParser::ToCubeMap(const gli::texture& _texture) {
		const auto& cubeMap = dynamic_cast<const gli::texture_cube&>(_texture);
		auto result = std::make_shared<CubeMap>(cubeMap.extent().x, GliFormatToTextureFormat(cubeMap.format()), cubeMap.levels());

		const char* data = reinterpret_cast<const char*>(cubeMap.data());
		gli::texture::size_type offset = 0;
		auto mipLevel = cubeMap.levels();

		for (uint32_t face = 0; face < 6; ++face) {
			for (uint32_t mip = 0; mip < mipLevel; ++mip) {
				result->setPixels(data + offset, cubeMap[face][mip].size(), static_cast<CubeMapFace>(face), mip);
				offset += cubeMap[face][mip].size();
			}
		}

		result->apply(false);

		return result;
	}

	TextureFormat GliParser::GliFormatToTextureFormat(const gli::format _format) {
		switch (_format) {
		case gli::FORMAT_RGBA8_UNORM_PACK8:
		case gli::FORMAT_RGBA8_UNORM_PACK32:
			return TextureFormat::R8G8B8A8_Unorm;
		case gli::FORMAT_BGRA8_UNORM_PACK8:
			return TextureFormat::B8G8R8A8_Unorm;
		default:
			return TextureFormat::Unknown;
		}
	}

}

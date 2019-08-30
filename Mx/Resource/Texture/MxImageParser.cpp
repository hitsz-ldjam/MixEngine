#include "MxImageParser.h"
#include "../../Log/MxLog.h"
#include "MxTexture.h"
#include <stb_image/stb_image.h>

namespace Mix {
	std::shared_ptr<ResourceBase> ImageParser::load(const std::filesystem::path& _path, const ResourceType _type, void* _additionalParam) {
		int width, height, channel;
		stbi_set_flip_vertically_on_load(true);
		auto data = stbi_load(_path.generic_string().c_str(), &width, &height, &channel, 4);
		if (data) {
			uint64_t size = width * height * 4;
			std::shared_ptr<Texture2D> result;

			if (_additionalParam) {
				TextureParam* param = reinterpret_cast<TextureParam*>(_additionalParam);
				result = std::make_shared<Texture2D>(width, height, TextureFormat::R8G8B8A8_UNORM, param->mipLevel, param->samplerInfo);
			}
			else
				result = std::make_shared<Texture2D>(width, height, TextureFormat::R8G8B8A8_UNORM);

			result->setPixels(reinterpret_cast<const char*>(data), size);
			result->apply(true);

			stbi_image_free(data);
			return result;
		}
		else {
			Log::Error("Failed to load image: %s", _path.generic_string().c_str());
			return nullptr;
		}
	}

	std::shared_ptr<ResourceBase> ImageParser::load(const std::filesystem::path& _path, const std::string& _ext, void* _additionalParam) {
		// we don't use paramater _ext and don't care about type
		return load(_path, ResourceType::Unknown, _additionalParam);
	}

}

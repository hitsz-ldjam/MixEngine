#pragma once
#ifndef MX_TEXTURE_PARSER_BASE_H_
#define MX_TEXTURE_PARSER_BASE_H_

#include "../MxResourceParserBase.hpp"
#include "MxTexture.h"

namespace Mix {
	namespace Resource {
		class TextureParserBase :public ResourceParserBase {
		protected:
			TextureParserBase() = default;

			static std::shared_ptr<Texture::DataType> getTextureDataStruct(const Texture& _texture) {
				return _texture.mData;
			}
		};
	}
}

#endif

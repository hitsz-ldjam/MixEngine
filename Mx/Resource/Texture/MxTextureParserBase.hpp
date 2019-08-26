#pragma once
#ifndef MX_TEXTURE_PARSER_BASE_H_
#define MX_TEXTURE_PARSER_BASE_H_

#include "../MxResourceParserBase.hpp"
#include "MxTexture.h"

namespace Mix {
	class TextureParserBase :public ResourceParserBase {
	protected:
		TextureParserBase() = default;
	};
}

#endif

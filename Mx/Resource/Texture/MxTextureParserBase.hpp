#pragma once
#ifndef MX_TEXTURE_PARSER_BASE_H_
#define MX_TEXTURE_PARSER_BASE_H_

#include "../MxResourceParserBase.hpp"

namespace Mix {
	class TextureParserBase :public ResourceParserBase {
	protected:
		TextureParserBase() = default;
	};
}

#endif

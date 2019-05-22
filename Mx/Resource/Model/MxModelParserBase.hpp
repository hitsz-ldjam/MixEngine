#pragma once
#ifndef MX_MODEL_PARSER_BASE_H_
#define MX_MODEL_PARSER_BASE_H_

#include "../MxResourceParserBase.hpp"

namespace Mix {
    namespace Resource {
        class ModelParseBase :public ResourceParserBase {
        protected:
            explicit ModelParseBase(ResourceRefMgr* _mgr) :ResourceParserBase(_mgr) {}
        };
    }
}

#endif

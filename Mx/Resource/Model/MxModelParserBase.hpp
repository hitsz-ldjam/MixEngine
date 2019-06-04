#pragma once
#ifndef MX_MODEL_PARSER_BASE_H_
#define MX_MODEL_PARSER_BASE_H_

#include "../MxResourceParserBase.hpp"
#include "MxMesh.h"
#include "MxModel.h"

namespace Mix {
    namespace Resource {
        class ModelParseBase :public ResourceParserBase {
        protected:
			ModelParseBase() = default;

			static std::shared_ptr<Mesh::DataType> GetMeshDataStruct(const Mesh& _mesh) {
				return _mesh.mData;
            }

			static std::shared_ptr<Model::DataType> GetModeldataStruct(const Model& _model) {
				return _model.mData;
            }
        };
    }
}

#endif

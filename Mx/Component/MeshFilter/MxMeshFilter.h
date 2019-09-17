#pragma once
#ifndef MX_MESH_FILTER_H_
#define MX_MESH_FILTER_H_
#include "../MxComponent.h"
#include "../../Graphics/Mesh/MxMesh.h"

namespace Mix {
	class MeshFilter :public Component {
		MX_DECLARE_RTTI;
	public:
		std::shared_ptr<Mesh> getMesh() const {
			return mMesh;
		}

		void setMesh(std::shared_ptr<Mesh> _mesh) {
			mMesh = std::move(_mesh);
		}
	private:
		std::shared_ptr<Mesh> mMesh;

	};
}

#endif

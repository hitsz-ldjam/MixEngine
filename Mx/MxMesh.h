#pragma once
#ifndef MX_MESH_H_
#define MX_MESH_H_

#include "MxGameObject.h"
#include "MxComponent.h"
#include "../Resource/MxResModel.h"
#include <string>

namespace Mix {
    class Mesh {
    public:
        Mesh() = default;

        explicit Mesh(Resource::ResourceRef _ref) :mMeshRef(std::move(_ref)) {}

        uint32_t SubmeshCount() const {
            return mMeshRef.Get<Resource::ResMesh>()->submeshes.size();
        }

    private:
        Resource::ResourceRef mMeshRef = nullptr;
    };

    class MeshFilter :public Component {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        MeshFilter() = default;

        void SetMesh(std::shared_ptr<Mesh> _mesh) {
            mMesh = _mesh;
        }

        std::shared_ptr<Mesh> GetMesh() const {
            return mMesh;
        }

    private:
        std::shared_ptr<Mesh> mMesh;
    };
}

#endif // !_MX_MESH_H_
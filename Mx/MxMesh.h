#pragma once
#ifndef MX_MESH_H_
#define MX_MESH_H_

#include "MxGameObject.h"
#include "MxComponent.h"
#include "../Resource/Model/MxResModel.h"

namespace Mix {
    class Mesh {
    public:
        Mesh() = default;

        explicit Mesh(Resource::ResourceRef _ref) :mMeshRef(std::move(_ref)) {}

        uint32_t submeshCount() const {
            return mMeshRef.dynamicCast<Resource::ResMesh*>()->submeshes.size();
        }

        const Resource::ResourceRef& meshRef() const {
            return mMeshRef;
        }
    private:
        Resource::ResourceRef mMeshRef = nullptr;
    };

    class MeshFilter :public Component {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        MeshFilter() = default;

        void setMesh(const std::shared_ptr<Mesh>& _mesh) {
            mMesh = _mesh;
        }

        std::shared_ptr<Mesh> getMesh() const {
            return mMesh;
        }

    private:
        std::shared_ptr<Mesh> mMesh;
    };
}

#endif // !_MX_MESH_H_
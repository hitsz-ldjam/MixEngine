#pragma once
#ifndef _MX_MESH_H_
#define _MX_MESH_H_

#include "MxGameObject.h"
#include "MxComponent.h"
#include <string>

namespace Mix {
    class MeshFilter :public Component {
        MX_DECLARE_RTTI;
        MX_DECLARE_CLASS_FACTORY;
    public:
        struct MeshRef {
            ModelId model;
            MeshId mesh;

            MeshRef() = default;
            MeshRef(const ModelId& model, const MeshId& mesh) :model(model), mesh(mesh) {}
        };

    public:
        const MeshRef& getMeshRef() const {
            return mMeshRef;
        }

        void setMeshRef(const ModelId & modelId, const MeshId & meshId) {
            // todo check if meshName is valid
            mMeshRef.model = modelId;
            mMeshRef.mesh = meshId;
        }
    private:
        // todo for now, we refer a mesh by name
        // we will change this in the future since this may be inefficient
        MeshRef mMeshRef;
    };
}

#endif // !_MX_MESH_H_
#include "MxColliderUtils.h"
#include "MxPhysicsUtils.hpp"
#include "../Resource/MxResourceLoader.h"
#include "../Resource/Model/MxModel.h"

namespace Mix::Physics {
    CompoundCollider createColliderFromFile(const std::string& _path,
                                            std::vector<ConvexHullCollider>& _children) {
        auto model = ResourceLoader::Get()->load<Model>(_path);
        if(!model)
            return nullptr;

        auto& meshes = model->getMeshes();

        CompoundCollider collider(new btCompoundShape(true, meshes.size()));

        _children.clear();
        _children.reserve(meshes.size());

        btTransform identityTrans(btTransform::getIdentity());

        for(const auto& mesh : meshes) {
            const auto& vertices = mesh->getPositions();

            ConvexHullCollider childShape(new btConvexHullShape);
            for(const auto& v : vertices)
                childShape->addPoint(Physics::mx_bt_cast(v), false);
            childShape->recalcLocalAabb();
            _children.emplace_back(childShape);

            collider->addChildShape(identityTrans, childShape.get());
        }

        return collider;
    }
}

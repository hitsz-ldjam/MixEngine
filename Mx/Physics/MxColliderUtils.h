#pragma once

#ifndef MX_COLLIDER_UTILS_H_
#define MX_COLLIDER_UTILS_H_

#include <bullet3/btBulletCollisionCommon.h>
#include <vector>

namespace Mix::Physics {
    using Collider = std::shared_ptr<btCollisionShape>;

    using BoxCollider = std::shared_ptr<btBoxShape>;
    using CapsuleColliderX = std::shared_ptr<btCapsuleShapeX>;
    using CapsuleColliderY = std::shared_ptr<btCapsuleShape>;
    using CapsuleColliderZ = std::shared_ptr<btCapsuleShapeZ>;
    using CompoundCollider = std::shared_ptr<btCompoundShape>;
    using ConvexHullCollider = std::shared_ptr<btConvexHullShape>;
    using SphereCollider = std::shared_ptr<btSphereShape>;

    /**
     *  @brief Create a collider according to a model file.
     *  @param[in] _path Path to the file.
     *  @param[out] _children Children shapes of the compound shape.
     *  @return The collider built from the file. @code nullptr @endcode if file not found.
     *  @attention CompoundCollider does NOT manage the lifetime of its children shapes internally.
     */
    CompoundCollider createColliderFromFile(const std::string& _path,
                                            std::vector<ConvexHullCollider>& _children);
}

#endif

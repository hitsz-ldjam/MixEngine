#pragma once
#ifndef MX_RENDER_ELEMENT_H_
#define MX_RENDER_ELEMENT_H_

#include <vector>
#include <memory>
#include "../Definitions/MxDefinitions.h"
#include "../Scene/MxSceneObjectHandle.h"

namespace Mix {
    class Mesh;
    class Material;
    class Renderer;
    class Camera;


    /**
     * \brief Information used to collect render information from scene.
     */
    struct SceneRenderInfo {
        Camera* camera = nullptr;

        std::vector<Renderer*> renderers;
    };


    struct RenderElement {
        HTransform transform;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint32_t submesh;
    };
}

#endif

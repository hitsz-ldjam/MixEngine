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
    class Animation;

    struct FrameAnimInfo;

    /**
     * \brief Information used to collect render information from scene.
     */
    struct FrameSceneInfo {
        Camera* camera = nullptr;

        std::vector<Renderer*> renderers;
        std::vector<Animation*> animations;
    };

    struct FrameRenderInfo {
        FrameSceneInfo* sceneInfo;
        FrameAnimInfo* animInfo;
    };


    struct RenderElement {
        HTransform transform;
        std::shared_ptr<Mesh> mesh;
        std::shared_ptr<Material> material;
        uint32_t submesh;
    };
}

#endif

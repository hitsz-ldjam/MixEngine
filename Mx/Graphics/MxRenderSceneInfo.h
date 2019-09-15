#pragma once
#ifndef MX_RENDER_ELEMENT_H_
#define MX_RENDER_ELEMENT_H_
#include <vector>

namespace Mix {
    class Transform;
    class Mesh;
    class Material;
    class Renderer;
    class Camera;


    struct SceneRenderInfo {
        Camera* camera = nullptr;
        std::vector<Renderer*> renderers;
    };
}

#endif

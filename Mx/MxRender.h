#pragma once
#ifndef _MX_RENDER_H_
#define _MX_RENDER_H_

#include <utility>
#include "MxMesh.h"
#include "../MxVk/Buffers/MxVkUniform.h"

namespace Mix {
    class MeshRenderer :public Component {
    public:

    private:
        Graphics::Uniform::MeshUniform mUniform;
    };
}

#endif // !_MX_RENDER_H_

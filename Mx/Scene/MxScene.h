#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include "../Window/MxWindow.h"

namespace Mix {
    // todo: temp demo scene
    class Scene {
    public:
        Scene();
        ~Scene();

        void init();
        void update();
        void fixedUpate();
        void lateUpate();

    private:
        Window* window;
    };
}

#endif

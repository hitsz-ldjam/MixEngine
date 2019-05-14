#pragma once

#ifndef MX_SCENE_H_
#define MX_SCENE_H_

#include "../Window/MxWindow.h"

namespace Mix {
    // todo: temp scene
    class Scene {
    public:
        void init();
        void update();

    private:
        Window fuckGarbageCPPWindow;
    };
}

#endif

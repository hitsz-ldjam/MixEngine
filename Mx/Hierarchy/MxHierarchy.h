#pragma once

#ifndef _MX_HIERARCHY_H
#define _MX_HIERARCHY_H

#include <iostream>
#include "../Input/MxInput.h"
#include "../Window/MxWindow.h"

namespace Mix {
    // todo: load hierarchy from XML file
    class Hierarchy {
    public:
        void init();
        void update();

    private:
        Window fuckGarbageCPPWindow;
    };
}

#endif

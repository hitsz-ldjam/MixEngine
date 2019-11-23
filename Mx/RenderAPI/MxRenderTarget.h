#pragma once
#ifndef MX_RENDER_TARGET_H_
#define MX_RENDER_TARGET_H_
#include "../Definitions/MxTypes.h"
#include "../Utils/MxEvent.h"
#include "../Math/MxRect.h"

namespace Mix {
    struct RenderTargetInfo {
        uint32 width = 0;
        uint32 height = 0;
        bool gamma = false;
        uint32 multisamleCount = 1;
    };

    class RenderTarget {
    public:
        RenderTarget();

        virtual ~RenderTarget() = default;

        virtual RenderTargetInfo getInfo() const = 0;

        // The first Rect2i is the old extent of this RenderTarget, 
        // the second Rect2i is the new extent of this RenderTarget
        mutable Event<void(Rect2i, Rect2i)> ResizedEvent;

    protected:

    };
}

#endif

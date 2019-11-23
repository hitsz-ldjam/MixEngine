#pragma once
#ifndef MX_VIEWPORT_H_
#define MX_VIEWPORT_H_
#include "../Math/MxRect.h"

namespace Mix {
    class Viewport {
    public:
        /**
         * \brief Contruct a Viewport with normalized extent
         * 
         * \note The value that is out of range [0, 1] will be clamped.
        */
        Viewport(float _x = 0.0f, float _y = 0.0f, float _width = 1.0f, float _height = 1.0f) {
            setRect(Rect2f(_x, _y, _width, _height));
        }

        Viewport& setRect(const Rect2f& _rect) {
            mNormRect.x = std::clamp(_rect.x, 0.0f, 1.0f);
            mNormRect.y = std::clamp(_rect.y, 0.0f, 1.0f);
            mNormRect.width = std::clamp(_rect.width, 0.0f, 1.0f);
            mNormRect.height = std::clamp(_rect.height, 0.0f, 1.0f);
            return *this;
        }

        const Rect2f& getRect() const { return mNormRect; }

    private:
        Rect2f mNormRect;
    };
}

#endif

#pragma once
#ifndef MX_RECT_H_
#define MX_RECT_H_

#include "../Definitions/MxTypes.h"
#include "MxVector2.h"
#include "MxMatrix4.h"

namespace Mix {
    struct Rect2i {
        constexpr Rect2i() = default;

        constexpr Rect2i(int32 _x, int32 _y, uint32 _w, uint32 _h) :
            x(_x), y(_y), width(_w), height(_h) {
        }

        template<typename _Ty>
        bool contains(const Vector2<_Ty>& _point) const;

        bool overlaps(const Rect2i& _other) const;

        void transform(const Matrix4& _mat);

        bool operator==(const Rect2i& _rhs) const {
            return x == _rhs.x &&
                y == _rhs.y &&
                width == _rhs.width &&
                height == _rhs.height;
        }

        bool operator!=(const Rect2i& _rhs) const {
            return !(*this == _rhs);
        }

        int32 x = 0;
        int32 y = 0;
        uint32 width = 0;
        uint32 height = 0;

        static const Rect2i Empty;
    };

    template <typename _Ty>
    bool Rect2i::contains(const Vector2<_Ty>& _point) const {
        return _point.x >= x && _point.x <= x + width &&
            _point.y >= y && _point.y <= y + height;
    }


    struct Rect2f {
        constexpr Rect2f() = default;

        constexpr Rect2f(float _x, float _y, float _width, float _height) :
            x(_x), y(_y), width(_width), height(_height) {
        }

        template<typename _Ty>
        bool contains(const Vector2<_Ty>& _point) const;

        bool overlaps(const Rect2f& _other) const;

        void transform(const Matrix4& _mat);

        Vector2f getCenter() const;

        Vector2f getHalfExtent() const;

        bool operator==(const Rect2f& _rhs) const {
            return x == _rhs.x &&
                y == _rhs.y &&
                width == _rhs.width &&
                height == _rhs.height;
        }

        bool operator!=(const Rect2f& _rhs) const {
            return !(*this == _rhs);
        }

        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;

        static const Rect2f Empty;
    };

    template <typename _Ty>
    bool Rect2f::contains(const Vector2<_Ty>& _point) const {
        return _point.x >= x && _point.x <= x + width &&
            _point.y >= y && _point.y <= y + height;
    }
}

#endif

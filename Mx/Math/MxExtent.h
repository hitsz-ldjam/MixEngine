#pragma once
#ifndef MX_EXTENT_H_
#define MX_EXTENT_H_
#include <cstdint>

namespace Mix {
    template<typename _Ty, uint32_t _N>
    struct TExtent;


    template<typename _Ty>
    struct TExtent<_Ty, 2> {
        TExtent() = default;

        TExtent(_Ty _width, _Ty _height) :
            width(_width),
            height(_height) {
        }

        _Ty area() const { return width * height; }

        _Ty width = _Ty(0);
        _Ty height = _Ty(0);
    };


    template<typename _Ty>
    struct TExtent<_Ty, 3> {
        TExtent() = default;

        TExtent(_Ty _width, _Ty _height, _Ty _depth) :
            width(_width),
            height(_height),
            depth(_depth) {
        }

        _Ty volume() const { return width * height * depth; }

        _Ty width = _Ty(0);
        _Ty height = _Ty(0);
        _Ty depth = _Ty(0);
    };

    template<typename _Ty>
    using Extent2D = TExtent<_Ty, 2>;

    template<typename _Ty>
    using Extent3D = TExtent<_Ty, 3>;

    using Extent2Df = Extent2D<float>;
    using Extent3Df = Extent3D<float>;

    using Extent2Di = Extent2D<int32_t>;
    using Extent3Di = Extent3D<int32_t>;

    using Extent2Dui = Extent2D<uint32_t>;
    using Extent3Dui = Extent3D<uint32_t>;

}

#endif

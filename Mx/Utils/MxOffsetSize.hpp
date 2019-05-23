#pragma once
#ifndef MX_UTILS_OFFSETSIZE_H_
#define MX_UTILS_OFFSETSIZE_H_
#include <cstdint>

namespace Mix {
    namespace Utils {
        template<typename _Ty = uint32_t>
        struct OffsetSize {
            using ValueType = _Ty;

            OffsetSize() = default;

            OffsetSize(ValueType const& _offset, ValueType const& _size) :offset(_offset), size(_size) {}

            bool operator==(OffsetSize const& _other) { return offset == _other.offset && size == _other.size; }

            bool operator!=(OffsetSize const& _other) { return !*this == _other; }

            ValueType offset = 0;
            ValueType size = 0;
        };
    }
}

#endif

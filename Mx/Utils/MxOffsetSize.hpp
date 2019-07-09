#pragma once
#ifndef MX_UTILS_OFFSETSIZE_H_
#define MX_UTILS_OFFSETSIZE_H_
#include <cstdint>

namespace Mix {
	template<typename _Ty>
	struct BasicOffsetSize {
		using ValueType = _Ty;

		BasicOffsetSize() = default;

		BasicOffsetSize(ValueType const& _offset, ValueType const& _size) :offset(_offset), size(_size) {}

		bool operator==(BasicOffsetSize const& _other) { return offset == _other.offset && size == _other.size; }

		bool operator!=(BasicOffsetSize const& _other) { return !*this == _other; }

		ValueType offset = 0;
		ValueType size = 0;
	};

	using OffsetSize32 = BasicOffsetSize<uint32_t>;
	using OffsetSize64 = BasicOffsetSize<uint64_t>;
}

#endif

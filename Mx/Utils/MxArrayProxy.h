#pragma once
#ifndef MX_ARRAY_PROXY_H_
#define MX_ARRAY_PROXY_H_

#include "../Definitions/MxDefinitions.h"

#include <cstdint>
#include <array>
#include <vector>
#include <cassert>

namespace Mix {
	template <typename _Ty>
	class ArrayProxy {
	public:
		constexpr ArrayProxy(std::nullptr_t)
			: mCount(0)
			, mPtr(nullptr) {
		}

		ArrayProxy(_Ty& _ptr)
			: mCount(1)
			, mPtr(&_ptr) {
		}

		ArrayProxy(const uint32_t _count, _Ty* _ptr)
			: mCount(_count)
			, mPtr(_ptr) {
		}

		template <size_t _N>
		ArrayProxy(std::array<typename std::remove_const<_Ty>::type, _N>& _data)
			: mCount(_N)
			, mPtr(_data.data()) {
		}

		template <size_t _N>
		ArrayProxy(std::array<typename std::remove_const<_Ty>::type, _N> const& _data)
			: mCount(_N)
			, mPtr(_data.data()) {
		}

		template <class Allocator = std::allocator<typename std::remove_const<_Ty>::type>>
		ArrayProxy(std::vector<typename std::remove_const<_Ty>::type, Allocator>& _data)
			: mCount(static_cast<uint32_t>(_data.size()))
			, mPtr(_data.data()) {
		}

		template <class Allocator = std::allocator<typename std::remove_const<_Ty>::type>>
		ArrayProxy(std::vector<typename std::remove_const<_Ty>::type, Allocator> const& _data)
			: mCount(static_cast<uint32_t>(_data.size()))
			, mPtr(_data.data()) {
		}

		ArrayProxy(std::initializer_list<_Ty> const& _data)
			: mCount(static_cast<uint32_t>(_data.end() - _data.begin()))
			, mPtr(_data.begin()) {
		}

		_Ty const* begin() const {
			return mPtr;
		}

		_Ty const* end() const {
			return mPtr + mCount;
		}

		_Ty const& front() const {
			MX_ASSERT(mCount && mPtr);
			return *mPtr;
		}

		_Ty const& back() const {
			MX_ASSERT(mCount && mPtr);
			return *(mPtr + mCount - 1);
		}

		bool empty() const {
			return (mCount == 0);
		}

		uint32_t size() const {
			return mCount;
		}

		_Ty* data() const {
			return mPtr;
		}

		_Ty const& operator[](size_t _index) const {
			return mPtr[_index];
		}

		_Ty& operator[](size_t _index) {
			return const_cast<_Ty&>(static_cast<ArrayProxy const&>(*this)[_index]);
		}

	private:
		uint32_t mCount;
		_Ty* mPtr;
	};
}

#endif

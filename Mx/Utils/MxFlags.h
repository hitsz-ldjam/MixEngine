#pragma once
#ifndef MX_FLAGS_H_
#define MX_FLAGS_H_
#include <cstdint>
#include <type_traits>

namespace Mix {
	template<typename _EnumType, typename _Enable = void> struct IsFlag :public std::false_type {};

	template<typename _EnumType, typename _MaskType = uint32_t>
	class Flags {
	public:
		static_assert(IsFlag<_EnumType>::value, "Flags is not allowed for this enum type. Use MX_ALLOW_FLAGS_FOR_ENUM macro");

		using EnumType = typename ::std::decay<_EnumType>::type;
		using UnderlyingType = std::underlying_type_t<_EnumType>;
		using MaskType = _MaskType;

		constexpr Flags()
			: mMask(0) {
		}

		Flags(_EnumType _bit)
			: mMask(static_cast<_MaskType>(_bit)) {
		}

		Flags(Flags<_EnumType, _MaskType> const& _rhs)
			: mMask(_rhs.mMask) {
		}

		explicit Flags(_MaskType _flags)
			: mMask(_flags) {
		}

		Flags<_EnumType, _MaskType>& operator=(Flags<_EnumType, _MaskType> const& _rhs) {
			mMask = _rhs.mMask;
			return *this;
		}

		Flags<_EnumType, _MaskType>& operator|=(Flags<_EnumType, _MaskType> const& _rhs) {
			mMask |= _rhs.mMask;
			return *this;
		}

		Flags<_EnumType, _MaskType>& operator&=(Flags<_EnumType, _MaskType> const& _rhs) {
			mMask &= _rhs.mMask;
			return *this;
		}

		Flags<_EnumType, _MaskType>& operator^=(Flags<_EnumType, _MaskType> const& _rhs) {
			mMask ^= _rhs.mMask;
			return *this;
		}

		Flags<_EnumType, _MaskType> operator|(Flags<_EnumType, _MaskType> const& _rhs) const {
			Flags<_EnumType, _MaskType> result(*this);
			result |= _rhs;
			return result;
		}

		Flags<_EnumType, _MaskType> operator&(Flags<_EnumType, _MaskType> const& _rhs) const {
			Flags<_EnumType, _MaskType> result(*this);
			result &= _rhs;
			return result;
		}

		Flags<_EnumType, _MaskType> operator^(Flags<_EnumType, _MaskType> const& _rhs) const {
			Flags<_EnumType, _MaskType> result(*this);
			result ^= _rhs;
			return result;
		}

		bool operator!() const {
			return !mMask;
		}

		Flags<_EnumType, _MaskType> operator~() const {
			return Flags(~mMask);
		}

		bool operator==(Flags<_EnumType, _MaskType> const& _rhs) const {
			return mMask == _rhs.mMask;
		}

		bool operator!=(Flags<_EnumType, _MaskType> const& _rhs) const {
			return mMask != _rhs.mMask;
		}

		explicit operator bool() const {
			return !!mMask;
		}

		explicit operator _MaskType() const {
			return mMask;
		}

		bool isSet(_EnumType _enum) const {
			return (mMask & static_cast<_MaskType>(_enum)) == static_cast<_MaskType>(_enum);
		}

		bool isAllSet(Flags<_EnumType, _MaskType> const& _flags) const {
			return (mMask & _flags.mMask) == _flags.mMask;
		}

		bool isAnySet(Flags<_EnumType, _MaskType> const& _flags) const {
			return (mMask & _flags.mMask) != 0;
		}

		auto& set(_EnumType _enum) {
			mMask |= static_cast<_MaskType>(_enum);
			return *this;
		}

		auto& unset(_EnumType _enum) {
			mMask &= ~static_cast<_MaskType>(_enum);
			return *this;
		}

	private:
		_MaskType  mMask;
	};
}

#ifndef MX_ALLOW_FLAGS_FOR_ENUM
#define MX_ALLOW_FLAGS_FOR_ENUM(name) MX_ALLOW_FLAGS_FOR_ENUM_EXT(name, uint32_t)
#endif

#ifndef MX_ALLOW_FLAGS_FOR_ENUM_EXT
#define MX_ALLOW_FLAGS_FOR_ENUM_EXT(name, type) \
template<> struct ::Mix::IsFlag< name >: public std::true_type{static_assert(std::is_enum< name >::value, "This macro is for Enum only");}; \
inline Flags< name , type > operator|(name a, name b) { Flags< name, type > r(a); r |= b; return r; } \
inline Flags< name , type > operator&(name a, name b) { Flags< name, type > r(a); r &= b; return r; } \
inline Flags< name , type > operator~(name a) { return ~Flags< name, type >(a); }
#endif

#endif

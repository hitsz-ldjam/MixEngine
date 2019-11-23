#pragma once
#ifndef MX_UTILS_GUID_H_
#define MX_UTILS_GUID_H_
#include <string>
#include "MxUtils.h"

namespace Mix {
	// From Bsf Framework
	class UUID {
	public:
		constexpr UUID() = default;

		constexpr UUID(uint32_t _part1, uint32_t _part2, uint32_t _part3, uint32_t _part4)
			:mData{ _part1,_part2,_part3,_part4 } {
		}

		explicit UUID(const std::string& _uuid);

		constexpr bool operator==(const UUID& _rhs) {
			return mData[0] == _rhs.mData[0] && mData[1] == _rhs.mData[1] && mData[2] == _rhs.mData[2] && mData[3] == _rhs.mData[3];
		}

		constexpr bool operator!=(const UUID& _rhs) {
			return !(*this == _rhs);
		}

		constexpr bool empty() const {
			return mData[0] == 0 && mData[1] == 0 && mData[2] == 0 && mData[3] == 0;
		}

		std::string toString() const;

		static UUID RandomUUID();
	private:
		friend struct std::hash<UUID>;

		uint32_t mData[4] = { 0,0,0,0 };
	};
}

namespace std {
	template<>
	struct hash<Mix::UUID> {
		size_t operator()(const Mix::UUID& _keyVal) const {
			size_t result = 0;
			Mix::Utils::HashCombine(result, _keyVal.mData[0]);
			Mix::Utils::HashCombine(result, _keyVal.mData[1]);
			Mix::Utils::HashCombine(result, _keyVal.mData[2]);
			Mix::Utils::HashCombine(result, _keyVal.mData[3]);

			return result;
		}
	};
}

#endif

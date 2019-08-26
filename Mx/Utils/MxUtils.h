#pragma once
#ifndef MX_UTILS_H_
#define MX_UTILS_H_

#include <filesystem>

#define MX_TO_STRING(a) (#a)
namespace Mix {
	namespace Utils {
		template<typename... _Args>
		static std::string StringFormat(const std::string &_format, _Args &&... _args) {
			const size_t size = snprintf(nullptr, 0, _format.c_str(), _args ...) + 1; // Extra space for '\0'
			std::unique_ptr<char[]> buf(new char[size]);
			snprintf(buf.get(), size, _format.c_str(), _args ...);
			return std::string(buf.get(), buf.get() + size - 1); // Excludes the '\0'
		}

		std::filesystem::path GetGenericPath(const std::string& _file);

		template <class T>
		void HashCombine(std::size_t& _seed, const T& _v) {
			std::hash<T> hasher;
			_seed ^= hasher(_v) + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
		}
	}
}
#endif // !_MX_UTILS_H_

#pragma once
#ifndef MX_UTILS_H_
#define MX_UTILS_H_

#include <iostream>
#include <filesystem>
#include <boost/format.hpp>

#define MX_TO_STRING(a) (#a)
namespace Mix {
    namespace Utils {
        template<typename... _Args>
        static std::string StringFormat(const std::string &_format, _Args &&... _args) {
            std::string result;
            try {
                boost::format f(_format);
                int a[] = { (f % _args,0)... };
                result = f.str();
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
            return result;
        }

        static std::string StringFormat(const std::string& _format) {
            return _format;
        }

        std::filesystem::path GetGenericPath(const std::string& _file);

        struct EnumHash {
            template<typename _Ty>
            constexpr size_t operator()(_Ty _e) const {
                return static_cast<std::size_t>(_e);
            }
        };

        template<typename _Ty>
        size_t Hash(_Ty const& _v) {
            using Hasher = std::conditional_t<std::is_enum_v<_Ty>, EnumHash, std::hash<_Ty>>;

            Hasher hasher;
            return hasher(_v);
        }

        template <class _Ty>
        void HashCombine(std::size_t& _seed, _Ty const& _v) {
            using Hasher = std::conditional_t<std::is_enum_v<_Ty>, EnumHash, std::hash<_Ty>>;

            Hasher hasher;
            _seed ^= hasher(_v) + 0x9e3779b9 + (_seed << 6) + (_seed >> 2);
        }
    }
}
#endif // !_MX_UTILS_H_

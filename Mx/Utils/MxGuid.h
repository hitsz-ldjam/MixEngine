#pragma once
#ifndef MX_UTILS_GUID_H_
#define MX_UTILS_GUID_H_
#include <string>
#include <filesystem>

namespace Mix {
    namespace Utils {
        class GuidGenerator {
        public:
            GuidGenerator() = delete;

            /**
             * \brief Generate a Guid
             */
            static long long GetGuid();

            /**
             * \brief Generate a Guid from string
             */
            static long long GetGuid(const std::string& _str);


            /**
             * \brief Generate a Guid from file path
             */
            static long long GetGuid(const std::filesystem::path& _path) {
                return hash_value(_path);
            }

        private:
            static std::hash<std::string> StringHash;
            static std::hash<long long> TimeHash;
        };
    }
}

#endif

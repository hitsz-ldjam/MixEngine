#include "MxGuid.h"

namespace Mix {
    namespace Utils {
        std::hash<long long> GuidGenerator::TimeHash;
        std::hash<std::string> GuidGenerator::StringHash;

        long long GuidGenerator::GetGuid() {
            return TimeHash(std::chrono::steady_clock::now().time_since_epoch().count());
        }

        long long GuidGenerator::GetGuid(const std::string& _str) {
            return StringHash(_str);
        }
    }
}
#pragma once
#ifndef MX_UTILS_H_
#define MX_UTILS_H_

#include <filesystem>

#define TO_STRING(a) (#a)

namespace Mix {
    namespace Utils {
        std::filesystem::path GetGenericPath(const std::string& _file);
        
    }
}
#endif // !_MX_UTILS_H_

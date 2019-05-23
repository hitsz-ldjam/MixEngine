#include "MxUtils.h"
#include <boost/algorithm/string.hpp>

namespace Mix {
    namespace Utils {
        std::filesystem::path GetGenericPath(const std::string& _file) {
            return std::filesystem::absolute(boost::to_lower_copy(_file)).generic_string();
        }
    }
}
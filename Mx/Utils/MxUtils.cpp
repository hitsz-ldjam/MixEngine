#include "MxUtils.h"
#include <boost/algorithm/string.hpp>

namespace Mix {
    namespace Utils {
        std::filesystem::path GetGenericPath(const std::string& _file) {
            return std::filesystem::absolute(boost::to_lower_copy(_file)).generic_string();
        }

        uint32 GetIndexFormatSize(IndexFormat _format) {
            switch (_format) {
            case IndexFormat::UInt16: return 16;;
            case IndexFormat::UInt32: return 32;;
            default: return 0;
            }
        }
    }
}
#include "MxSystemInfo.h"
#include "MxDefinitions.h"

namespace Mix {
    const std::string SystemInfo::EngineName = MX_ENGINE_NAME;

    const Version SystemInfo::EngineVersion = Version(MX_ENGINE_VERSION_MAJOR, MX_ENGINE_VERSION_MINOR, MX_ENGINE_VERSION_PATCH);
}

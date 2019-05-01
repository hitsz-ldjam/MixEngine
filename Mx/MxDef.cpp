#include "MxDef.h"

namespace Mix {
    const std::string EngineInfo::engineName = MX_ENGINE_NAME;

    const VersionInt EngineInfo::engineVersion = Version::makeVersion(MX_ENGINE_VERSION_MAJOR,
                                                                      MX_ENGINE_VERSION_MINOR,
                                                                      MX_ENGINE_VERSION_PATCH);

}
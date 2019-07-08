#pragma once

#ifndef MX_DEFINITIONS_H_
#define MX_DEFINITIONS_H_

#include <cstdint>
#include <string>
#include <cstdint>

#ifndef MX_ENGINE_NAME
#define MX_ENGINE_NAME "MixEngine"
#endif

#ifndef MX_ENGINE_VERSION_MAJOR
#define MX_ENGINE_VERSION_MAJOR 0
#endif

#ifndef MX_ENGINE_VERSION_MINOR
#define MX_ENGINE_VERSION_MINOR 0
#endif

#ifndef MX_ENGINE_VERSION_PATCH
#define MX_ENGINE_VERSION_PATCH 1
#endif

#ifndef MX_ASSERT
#define MX_ASSERT assert	
#endif


namespace Mix {
    using VersionInt = uint32_t;
    using Guid = long long;

    namespace Version {
        static VersionInt MakeVersion(const uint32_t _major, const uint32_t _minor, const uint32_t _patch) {
            return (_major << 22 | _minor << 12 | _patch);
        }

        static VersionInt GetMajor(const VersionInt _version) {
            return _version >> 22;
        }

        static VersionInt GetMinor(const VersionInt _version) {
            return (_version >> 12) & 0x3ff;
        }

        static VersionInt GetPatch(const VersionInt _version) {
            return _version & 0xfff;
        }
    }

    namespace EngineInfo {
        static const std::string EngineName = MX_ENGINE_NAME;

        static const VersionInt EngineVersion = Version::MakeVersion(MX_ENGINE_VERSION_MAJOR,
                                                                     MX_ENGINE_VERSION_MINOR,
                                                                     MX_ENGINE_VERSION_PATCH);
    }

    enum class Space { WORLD, SELF };
}

#endif

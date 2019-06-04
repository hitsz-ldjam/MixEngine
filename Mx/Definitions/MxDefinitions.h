#pragma once

#ifndef MX_DEFINITIONS_H_
#define MX_DEFINITIONS_H_

#include <cstdint>
#include <string>

#define MX_ENGINE_NAME "MixEngine"
#define MX_ENGINE_VERSION_MAJOR 0
#define MX_ENGINE_VERSION_MINOR 0
#define MX_ENGINE_VERSION_PATCH 1

namespace Mix {
    typedef uint32_t VersionInt;
    typedef uint32_t LayerIndex;
    typedef std::string Tag;
    typedef long long Guid;

    //typedef vk::DeviceSize Id;
    //typedef vk::DeviceSize IdStep;
    //typedef Id MeshId;
    //typedef Id ModelId;

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

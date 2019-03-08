#pragma once
#ifndef _MX_DEF_H_
#define _MX_DEF_H_

#include <cstdint>
#include <string>

#define MX_ENGINE_NAME "MixEngine"
#define MX_ENGINE_VERSION_MAJOR 0
#define MX_ENGINE_VERSION_MINOR 0
#define MX_ENGINE_VERSION_PATCH 1


#define MX_FREE_POINTER(p)											\
	if((p)!=nullptr)												\
		delete (p);


namespace Mix {

    typedef uint32_t VersionInt;

    struct Version {
        static VersionInt makeVersion(uint32_t major, uint32_t minor, uint32_t patch) {
            return (major << 22 | minor << 12 | patch);
        }

        static VersionInt getMajor(VersionInt version) {
            return version >> 22;
        }

        static VersionInt getMinor(VersionInt version) {
            return (version >> 12) & 0x3ff;
        }

        static VersionInt getPatch(VersionInt version) {
            return version & 0xfff;
        }
    };

    struct EngineInfo {
        static const std::string engineName;

        static const VersionInt engineVersion;
    };

}

#endif // !_MX_DEF_H_

#pragma once
#ifndef MX_DEF_H_
#define MX_DEF_H_

#include <cstdint>
#include <string>

#include <vulkan/vulkan.hpp>

// include GLM
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


#define MX_ENGINE_NAME "MixEngine"
#define MX_ENGINE_VERSION_MAJOR 0
#define MX_ENGINE_VERSION_MINOR 0
#define MX_ENGINE_VERSION_PATCH 1


#define MX_FREE_POINTER(p)											\
	if((p)!=nullptr){												\
		delete (p);\
        (p)=nullptr;\
}


namespace Mix {

    typedef uint32_t VersionInt;
    typedef uint32_t LayerIndex;
    typedef std::string Tag;
    typedef vk::DeviceSize Id;
    typedef vk::DeviceSize IdStep;
    typedef Id MeshId;
    typedef Id ModelId;
    typedef long long Guid;

    namespace Version {
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

    namespace EngineInfo {
        static const std::string engineName = MX_ENGINE_NAME;

        static const VersionInt engineVersion = Version::makeVersion(MX_ENGINE_VERSION_MAJOR,
                                                                     MX_ENGINE_VERSION_MINOR,
                                                                     MX_ENGINE_VERSION_PATCH);
    };

    namespace Constants {
        constexpr float Epsilon = 0.000001f;
        constexpr float Pi = 3.141593f;
    }

    struct NoCopy {
        NoCopy() = default;

        ~NoCopy() = default;

        NoCopy(const NoCopy&) = delete;

        NoCopy(NoCopy&&) = default;

        NoCopy& operator=(const NoCopy&) = delete;

        NoCopy& operator=(NoCopy&&) = default;
    };
}

#endif // !MX_DEF_H_

#pragma once
#ifndef MX_VIDEO_MODE_H_
#define MX_VIDEO_MODE_H_
#include "../Definitions/MxTypes.h"
#include "../Utils/MxGeneralBase.hpp"

namespace Mix {
    struct VideoMode {
        VideoMode() = default;

        VideoMode(uint32 _width, uint32 _height, float _fps = 60.0f, uint32 _deviceIdx = 0)
            :width(_width), height(_height), fps(_fps), deviceIdx(_deviceIdx) {
        }

        bool operator==(const VideoMode& _other) const {
            return width == _other.width && height == _other.height && fps == _other.fps && deviceIdx == _other.deviceIdx;
        }

        bool operator!=(const VideoMode& _other) const {
            return !(*this == _other);
        }

        uint32 width;
        uint32 height;
        float fps;
        uint32 deviceIdx = 0;
    };

    class DeviceVideoMode :public GeneralBase::NoCopyBase {
    public:
        DeviceVideoMode() = default;

        const std::string& getDeviceName() const { return mName; }

        uint32 getVideoModeCount() const { return uint32(mVideoModes.size()); }

        const VideoMode& getVideoMode(uint32 _index) const { return mVideoModes.at(_index); }

        const VideoMode& getDesktopVideoMode() const { return mVideoModes[mDesktopVideoModeIndex]; }

    protected:
        std::string mName;
        std::vector<VideoMode> mVideoModes;
        uint32 mDesktopVideoModeIndex = 0;
    };

    class DeviceVideoModeSet :public GeneralBase::NoCopyBase {
    public:
        DeviceVideoModeSet() = default;

        uint32 getDeviceCount() const { return uint32(mDeviceVideoModes.size()); }

        const DeviceVideoMode& getDeviceVideoMode(uint32 _index)const { return mDeviceVideoModes.at(_index); }

    private:
        std::vector<DeviceVideoMode> mDeviceVideoModes;
    };
}

#endif

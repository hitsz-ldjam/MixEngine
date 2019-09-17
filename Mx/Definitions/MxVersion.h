#pragma once
#ifndef MX_VERSION_H_
#define MX_VERSION_H_
#include <cstdint>

namespace Mix {
    class Version {
    public:
        Version() = default;

        Version(uint32_t _major, uint32_t _minor, uint32_t _patch)
            :mMajor(_major), mMinor(_minor), mPatch(_patch) {
        }

        uint32_t getMajor() const { return mMajor; }

        void setMajor(uint32_t _major) { mMajor = _major; };

        uint32_t getMinor() const { return mMinor; }

        void setMinor(uint32_t _minor) { mMinor = _minor; }

        uint32_t getPatch() const { return mPatch; }

        void setPatch(uint32_t _patch) { mPatch = _patch; }

    private:
        uint32_t mMajor = 0;
        uint32_t mMinor = 0;
        uint32_t mPatch = 0;
    };
}

#endif

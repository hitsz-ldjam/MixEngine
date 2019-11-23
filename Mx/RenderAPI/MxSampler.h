#pragma once
#ifndef MX_SAMPLER_STATE_H_
#define MX_SAMPLER_STATE_H_

#include "../Definitions/MxCommonEnum.h"
#include "../Definitions/MxTypes.h"
#include <limits>

namespace Mix {
    struct SamplerDesc {
        bool operator==(const SamplerDesc& _other) const {
            return minFilter == _other.minFilter
                   && magFilter == _other.magFilter
                   && mipFilter == _other.mipFilter
                   && wrapModeU == _other.wrapModeU
                   && wrapModeV == _other.wrapModeV
                   && wrapModeW == _other.wrapModeW
                   && maxAniso == _other.maxAniso
                   && mipMin == _other.mipMin
                   && mipMax == _other.mipMax
                   && mipmapBias == _other.mipmapBias;
        }

        bool operator!=(const SamplerDesc& _other) const {
            return !(*this == _other);
        }

        TextureFilterMode minFilter = TextureFilterMode::Nearest;
        TextureFilterMode magFilter = TextureFilterMode::Nearest;
        TextureMipSampleMode mipFilter = TextureMipSampleMode::Nearest;

        TextureWrapMode wrapModeU = TextureWrapMode::Repeat;
        TextureWrapMode wrapModeV = TextureWrapMode::Repeat;
        TextureWrapMode wrapModeW = TextureWrapMode::Repeat;

        uint32 maxAniso = 0;

        float mipMin = 0.0f;

        float mipMax = std::numeric_limits<float>::infinity();

        float mipmapBias = 0.0f;
    };
}

#endif

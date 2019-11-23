#pragma once
#ifndef MX_RASTERIZER_STATE_H_
#define MX_RASTERIZER_STATE_H_
#include "MxVideoMode.h"

namespace Mix {
    struct RasterizerStateDesc {
        bool operator==(const RasterizerStateDesc& _other) const {
            return polygonMode == _other.polygonMode
                && cullMode == _other.cullMode
                && depthBiasEnable == _other.depthBiasEnable
                && depthBiasConstantFactor == _other.depthBiasConstantFactor
                && depthBiasSlopeFactor == _other.depthBiasSlopeFactor
                && depthBiasClamp == _other.depthBiasClamp;
        }

        bool operator!=(const RasterizerStateDesc& _other) const {
            return !(*this == _other);
        }

        PolygonMode polygonMode = PolygonMode::Triangle;

        CullMode cullMode = CullMode::None;

        bool depthBiasEnable = false;

        float depthBiasConstantFactor = 0.0f;

        float depthBiasSlopeFactor = 0.0f;

        float depthBiasClamp = 0.0f;
    };
}

#endif

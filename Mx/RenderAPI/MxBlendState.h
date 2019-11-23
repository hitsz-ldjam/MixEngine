#pragma once
#ifndef MX_BLEND_STATE_H_
#define MX_BLEND_STATE_H_
#include "../../Demos/DFJ/Scripts/EnemyShip.h"
#include "../Definitions/MxTypes.h"

namespace Mix {
    // todo multiple target blend state support
    struct TargetBlendStateDesc {
        bool operator==(const TargetBlendStateDesc& _other) const {
            return blendEnable == _other.blendEnable
                   && srcColorFactor == _other.srcColorFactor
                   && dstColorFactor == _other.dstColorFactor
                   && colorOp == _other.colorOp
                   && srcAlphaFactor == _other.srcAlphaFactor
                   && dstAlphaFactor == _other.dstAlphaFactor
                   && alphaOp == _other.alphaOp
                   && colorComponentFlags == _other.colorComponentFlags;
        }

        bool operator!=(const TargetBlendStateDesc& _other) const {
            return !(*this == _other);
        }

        bool blendEnable = false;

        BlendFactor srcColorFactor = BlendFactor::One;

        BlendFactor dstColorFactor = BlendFactor::Zero;

        BlendOp colorOp = BlendOp::Add;

        BlendFactor srcAlphaFactor = BlendFactor::One;

        BlendFactor dstAlphaFactor = BlendFactor::Zero;

        BlendOp alphaOp = BlendOp::Add;

        uint8 colorComponentFlags = uint32(ColorComponent::R | ColorComponent::G | ColorComponent::B | ColorComponent::A);
    };


    
}
#endif

#pragma once
#ifndef MX_DEPTH_STENCIL_STATE_H_
#define MX_DEPTH+STENCIL_STATE_H_
#include "../Definitions/MxCommonEnum.h"
#include "../Definitions/MxTypes.h"

namespace Mix {
    struct DepthStencilStateDesc {
        bool operator==(const DepthStencilStateDesc& _other) const {
            return depthTestEnable == _other.depthTestEnable
                && depthWriteEnable == _other.depthWriteEnable
                && depthCompareOp == _other.depthCompareOp
                && stencilEnable == _other.stencilEnable
                && frontStencilState == _other.frontStencilState
                && backStencilState == _other.backStencilState;
        }

        bool depthTestEnable = true;

        bool depthWriteEnable = true;

        CompareOp depthCompareOp = CompareOp::Less;

        bool stencilEnable = false;

        struct StencilStateDesc {
            bool operator==(const StencilStateDesc& _other) const {
                return failOp == _other.failOp
                    && passOp == _other.passOp
                    && depthFailOp == _other.depthFailOp
                    && compareOp == _other.compareOp
                    && compareMask == _other.compareMask
                    && writeMask == _other.writeMask
                    && reference == _other.reference;
            }

            bool operator!=(const StencilStateDesc& _other) const {
                return !(*this == _other);
            }

            StencilOp failOp = StencilOp::Keep;
            StencilOp passOp = StencilOp::Keep;
            StencilOp depthFailOp = StencilOp::Keep;
            CompareOp compareOp = CompareOp::Never;
            uint32 compareMask = 0;
            uint32 writeMask = 0;
            uint32 reference = 0;
        };

        StencilStateDesc frontStencilState;

        StencilStateDesc backStencilState;
    };
}

#endif

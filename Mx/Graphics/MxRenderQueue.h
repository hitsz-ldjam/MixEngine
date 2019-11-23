#pragma once
#ifndef MX_RENDER_QUEUE_H_
#define MX_RENDER_QUEUE_H_
#include <cstdint>
#include "../Definitions/MxCommonEnum.h"
#include "MxRenderInfo.h"

namespace Mix {
    struct RenderQueueElement {
        RenderElement* element;
        uint32_t shaderId;
    };

    class RenderQueue {
        struct SortElement {
            RenderElement* element;
            uint32_t shaderId;
            float disFromCamera;
        };

    public:
        enum SortType {
            SortType_FrontToBack,
            SortType_BackToFront
        };

        RenderQueue(SortType _type = SortType_FrontToBack);

        void push(RenderElement* _element, float _distFromCamera);

        void clear();

        void sort();

        const std::vector<RenderQueueElement>& getSortedElements() const;

    private:
        std::vector<SortElement> mSortElements;
        std::vector<size_t> mSortIndex; 
        std::vector<RenderQueueElement> mSortedElements;
        SortType mSortType;
    };
}

#endif

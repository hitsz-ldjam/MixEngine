#include "MxRenderQueue.h"
#include "MxMaterial.h"

namespace Mix {

    RenderQueue::RenderQueue(SortType _type) :mSortType(_type) {
    }

    void RenderQueue::push(RenderElement* _element, float _distFromCamera) {
        SortElement element;
        element.element = _element;
        element.disFromCamera = mSortType == SortType_FrontToBack ? _distFromCamera : -_distFromCamera;
        element.shaderId = _element->material->getShader()->getId();

        mSortIndex.push_back(mSortElements.size());
        mSortElements.push_back(element);
    }

    void RenderQueue::clear() {
        mSortElements.clear();
        mSortIndex.clear();
        mSortedElements.clear();
    }

    void RenderQueue::sort() {
        std::sort(mSortIndex.begin(), mSortIndex.end(),
                  [&](const size_t& _a, const size_t& _b) {
                      auto& eleA = mSortElements[_a];
                      auto& eleB = mSortElements[_b];

                      uint8_t h = (eleA.shaderId > eleB.shaderId) << 1 |
                          (eleA.disFromCamera > eleB.disFromCamera);

                      uint8_t l = (eleA.shaderId < eleB.shaderId) << 1 |
                          (eleA.disFromCamera < eleB.disFromCamera);

                      return l > h;
                  }
        );

        for (auto& idx : mSortIndex) {
            RenderQueueElement element;
            element.element = mSortElements[idx].element;
            element.shaderId = mSortElements[idx].shaderId;
            mSortedElements.push_back(element);
        }
    }

    const std::vector<RenderQueueElement>& RenderQueue::getSortedElements() const {
        return mSortedElements;
    }
}

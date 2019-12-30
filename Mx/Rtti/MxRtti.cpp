#include "MxRtti.hpp"

namespace Mix {

    bool Rtti::isDerivedFrom(const Rtti* _type) const {
        const Rtti* pTemp = this;

        //if (pTemp->isSameType(_type))
        //    return false;

        while (!pTemp->isSameType(_type)) {
            if (pTemp->mpBase) {
                pTemp = pTemp->mpBase;
            }
            else {
                return false;
            }
        }
        return true;
    }
}

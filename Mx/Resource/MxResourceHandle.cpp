#include "MxResourceHandle.h"
#include "MxResourceBase.h"

namespace Mix {
    bool ResourceHandleBase::isLoaded(bool _checkDependencies) const {
    }

    void ResourceHandleBase::waitUtilLoaded() const {
    }

    void ResourceHandleBase::release() {
    }

    const UUID& ResourceHandleBase::getUUID() const {
    }

    void ResourceHandleBase::destroy() {
    }

    void ResourceHandleBase::setHandleData(const std::shared_ptr<Resource>& _resource, const UUID& _uuid) {
    }

    void ResourceHandleBase::clearHandleData() {
    }

    void ResourceHandleBase::incrInternalRef() {
    }

    void ResourceHandleBase::decrInternalRef() {
    }

    void ResourceHandleBase::onLoadComplete() {
    }

    void ResourceHandleBase::throwIfNotLoaded() const {
        
    }
}

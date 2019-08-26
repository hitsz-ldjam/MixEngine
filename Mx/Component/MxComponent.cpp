#include "MxComponent.h"
#include "../GameObject/MxGameObject.h"

namespace Mix {
	MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(Component, Object)

		const Transform& Component::transform() const {
		return mGameObject->transform();
	}

	Transform& Component::transform() {
		return mGameObject->transform();
	}
}

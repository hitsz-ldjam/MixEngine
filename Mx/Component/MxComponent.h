#pragma once

#ifndef MX_COMPONENT_H_
#define MX_COMPONENT_H_

#include "../Object/MxObject.h"

namespace Mix {
	class GameObject;
	class Transform;

	class Component : public Object {
		MX_DECLARE_RTTI
			MX_DECLARE_NO_CLASS_FACTORY

			friend GameObject;

	public:
		Component() : mGameObject(nullptr) {}

		/**
		 *  @note Copy ctor should be called before engine awakes. And copy constructed Components
		 *  should be added to corresponding GameObjects manually via GameObject::addComponent.
		 */
		Component(const Component& _other) : Object(_other),
			mGameObject(nullptr) {
		}

		virtual ~Component() = 0 {}

		auto getGameObject() const noexcept { return mGameObject; }

		const Transform& transform() const;

		Transform& transform();

	protected:
		GameObject* mGameObject;
		void setGameObject(GameObject* _obj) noexcept { mGameObject = _obj; }
	};
}

#endif

#pragma once

#ifndef MX_COMPONENT_H_
#define MX_COMPONENT_H_

#include "../Scene/MxSceneObject.h"
#include "../Definitions/MxDefinitions.h"
#include "../Scene/MxSceneObjectHandle.h"

namespace Mix {
    class GameObject;
    class Transform;

    class Component : public SceneObject {
        MX_DECLARE_RTTI;
        friend GameObject;
        friend SceneManager;
    public:
        virtual ~Component() = default;

        const HGameObject& getGameObject() const noexcept { return mGameObject; }

        HGameObject& getGameObject() noexcept { return mGameObject; }

        const HComponent& getHandle() const { return mThisHandle; }

        void destroy(bool _immediate = false);

        HTransform transform() const;

        void destroyInternal(SceneObjectHandleBase& _handle, bool _immediate) override;

    protected:
        Component() = default;

        explicit Component(const HGameObject& _gameObject);

        HComponent mThisHandle;
        HGameObject mGameObject;
    };
}

#endif

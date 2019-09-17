#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "../Mx/Math/MxVector.h"
#include "../Mx/Utils/MxEvent.h"

namespace Scripts {
    using namespace Mix;

    class ShipAdapter;

    class PlayerShipScript :public Script {
        MX_DECLARE_RTTI;
    public:
        ~PlayerShipScript() = default;

    private:
        void init() override;

        void update() override;

        void fixedUpdate() override;

        Vector2f getDir();

        Vector2f dirFromGamepad();

        Vector2f dirFromKeyboard();

        SceneObjectHandle<ShipAdapter> mAdapter;
    };
}
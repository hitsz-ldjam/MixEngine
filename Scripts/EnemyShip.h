#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/GameObject/MxGameObject.h"

namespace Scripts {
    using namespace Mix;

    class EnemyShipScript :public Script {
        MX_DECLARE_RTTI;
    public:
        ~EnemyShipScript() = default;

    private:
        void init() override;

        void update() override;

        void fixedUpdate() override;

    };
}
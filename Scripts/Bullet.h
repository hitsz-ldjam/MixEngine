#pragma once
#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/Graphics/MxMaterial.h"

namespace Scripts {
    using namespace Mix;

    class Bullet :public Script {
        MX_DECLARE_RTTI;
    public:


    private:
        void awake() override;

        void init() override;

        void update() override;

        float mMoveSpeed = 1.0f;
    };
}

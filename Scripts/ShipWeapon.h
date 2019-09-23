#pragma once
#include "../Mx/Component/Script/MxScript.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/Graphics/Mesh/MxMesh.h"

namespace Scripts {
    using namespace Mix;

    class ShipWeapon :public Script {
        MX_DECLARE_RTTI;
    public:
        void attack();

        void setAttackSpeed(float _speed);

    private:
        void awake() override;

        void start() override;

        void update() override;

        void fire();

        std::shared_ptr<Mesh> mBulletMesh;
        std::shared_ptr<Material> mBulletMat;

        // Attack speed
        float mAttackGap = 0.0f;
        float mAttackSpeed = 10.0f;

        uint32_t mFireCount = 0;
    };
}

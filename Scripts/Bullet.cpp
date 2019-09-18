#include "Bullet.h"
#include "../Mx/Component/Transform/MxTransform.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/GameObject/MxGameObject.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(Bullet, Script);

    void Bullet::awake() {
    }

    void Bullet::init() {
    }

    void Bullet::update() {
        transform()->translate(0.0f, mMoveSpeed, 0.0f, Space::World);
        if (transform()->getPosition().y >= 10.f)
            mGameObject->destroy();
    }
}

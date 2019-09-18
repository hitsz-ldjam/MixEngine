#include "PlayerShip.h"
#include "../Mx/Input/MxInput.h"
#include "../Mx/Component/Camera/MxCamera.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/Window/MxWindow.h"
#include "ShipAdapter.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(PlayerShipScript, Script);

    void PlayerShipScript::init() {
        mAdapter = mGameObject->getComponent<ShipAdapter>();
    }

    void PlayerShipScript::update() {
        mAdapter->move(getDir());

        if (Input::Get()->isButtonHold(ButtonCode::Space) || Input::Get()->isButtonHold(ButtonCode::Gamepad_A))
            mAdapter->attack();
    }

    void PlayerShipScript::fixedUpdate() {
    }

    Vector2f PlayerShipScript::getDir() {
        auto dir = dirFromKeyboard() + dirFromGamepad();
        return !(dir.length() > 0.0f) ? Vector2f::Zero : dir.normalize();
    }

    Vector2f PlayerShipScript::dirFromGamepad() {
        auto dir = Input::Get()->getGamepadLeftStickAxis();
        return !(dir.length() > 0.0f) ? Vector2f::Zero : dir.normalize();
    }

    Vector2f PlayerShipScript::dirFromKeyboard() {
        Vector2f dir;
        if (Input::Get()->isButtonHold(ButtonCode::W)) {
            dir += Vector2f::Up;
        }
        if (Input::Get()->isButtonHold(ButtonCode::A)) {
            dir += Vector2f::Left;
        }
        if (Input::Get()->isButtonHold(ButtonCode::S)) {
            dir += Vector2f::Down;
        }
        if (Input::Get()->isButtonHold(ButtonCode::D)) {
            dir += Vector2f::Right;
        }
        return dir.length() > 0.0f ? dir.normalize() : Vector2f::Zero;
    }
}

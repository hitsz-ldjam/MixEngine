#include "MxInput.h"
#include "../Engine/MxPlatform.h"
#include "../Log/MxLog.h"
#include "MxGamepad.h"
#include "../../MixEngine.h"

using namespace std::placeholders;
namespace Mix {
    Input* Input::Get() {
        return MixEngine::Instance().getModule<Input>();
    }

    Input::~Input() {
        for (auto p : mGamepads)
            delete p;
    }

    void Input::load() {
        mKeyboard = std::make_unique<Keyboard>(this);
        mMouse = std::make_unique<Mouse>(this);

        Platform::MouseButtonEvent.connect(std::bind(&Input::onMouseButton, this, _1));
        Platform::MouseMoveEvent.connect(std::bind(&Input::onMouseMove, this, _1));
        Platform::MouseWheelEvent.connect(std::bind(&Input::onMouseWheel, this, _1));
        Platform::KeyboardEvent.connect(std::bind(&Input::onKeyboard, this, _1));
        Platform::GamepadAxisEvent.connect(std::bind(&Input::onGamepadAxis, this, _1));
        Platform::GamepadButtonEvent.connect(std::bind(&Input::onGamepadButton, this, _1));
        Platform::GamepadDeviceEvent.connect(std::bind(&Input::onGamepadDevice, this, _1));
    }

    bool Input::anyKeyDown() {
        return mAnyKeyDown;
    }

    float Input::getAxis(AxisCode _axis, int32_t _id) const {
        if (MX_AC_CHECK_MOUSE(_axis)) {
            return mMouse->getAxis(_axis);
        }
        else if (MX_AC_CHECK_GAMEPAD(_axis)) {
            auto gamepad = getGamepadFromId(_id);
            if (gamepad)
                return gamepad->getAxis(_axis);
        }
        return 0.0f;
    }

    Vector2f Input::getGamepadLeftStickAxis(int32_t _id) const {
        auto gamepad = getGamepadFromId(_id);
        if (gamepad)
            return gamepad->getAxis(AxisCode::Gamepad_LeftX, AxisCode::Gamepad_LeftY);
        return Vector2f::Zero;
    }

    Vector2f Input::getGamepadRightStickAxis(int32_t _id) const {
        auto gamepad = getGamepadFromId(_id);
        if (gamepad)
            return gamepad->getAxis(AxisCode::Gamepad_RightX, AxisCode::Gamepad_RightY);
        return Vector2f::Zero;
    }

    bool Input::isButtonHold(ButtonCode _button, int32_t _id) const {
        return getButtonState(_button, _id) & ButtonState_Hold;
    }

    bool Input::isButtonDown(ButtonCode _button, int32_t _id) const {
        return getButtonState(_button, _id) & ButtonState_Down;
    }

    bool Input::isButtonUp(ButtonCode _button, int32_t _id) const {
        return getButtonState(_button, _id) & ButtonState_Up;
    }

    const Vector2i& Input::getMouseWheelScrollDelta() const {
        return mMouse->getMouseWheelState();
    }

    const Vector2i& Input::getMousePosition() const {
        return mMouse->getPosState();
    }

    const Vector2i& Input::getMousePositionDelta() const {
        return mMouse->getPosDeltaState();
    }

    bool Input::isMouseDoubleClick() const {
        return false;
    }

    void Input::nextFrame() {
        mMouse->nextFrame();
        mKeyboard->nextFrame();
        for (auto p : mGamepads)
            p->nextFrame();
        mAnyKeyDown = false;
    }

    void Input::onMouseButton(const PFMouseButtonEventData& _data) {
        mMouse->updateButton(_data);
        if (_data.state & PFButtonState_Pressed)
            onAnyKeyDown();
    }

    void Input::onMouseMove(const PFMouseMoveEventData& _data) {
        mMouse->updatePos(_data);
    }

    void Input::onMouseWheel(const PFMouseWheelEventData& _data) {
        mMouse->updateWheel(_data);
    }

    void Input::onKeyboard(const PFKeyboardEventData& _data) {
        mKeyboard->updateButton(_data);
        if (_data.state & PFButtonState_Pressed)
            onAnyKeyDown();
    }

    void Input::onGamepadButton(const PFGamepadButtonEventData& _data) {
        auto it = std::find_if(mGamepads.begin(), mGamepads.end(), [&](const Gamepad* _p) {return _p->getInstanceId() == _data.id; });

        if (it != mGamepads.end())
            (*it)->updateButton(_data);

        if (_data.state & PFButtonState_Pressed)
            onAnyKeyDown();
    }

    void Input::onGamepadAxis(const PFGamepadAxisEventData& _data) {
        auto it = std::find_if(mGamepads.begin(), mGamepads.end(), [&](const Gamepad* _p) {return _p->getInstanceId() == _data.id; });

        if (it != mGamepads.end())
            (*it)->updateAxis(_data);
    }

    void Input::onGamepadDevice(const PFGamepadDeviceEventData& _data) {
        if (_data.deviceState & PFDeviceState_Added) {
            mGamepads.push_back(new Gamepad(_data.which, this));
        }
        else if (_data.deviceState & PFDeviceState_Removed) {
            auto it = std::find_if(mGamepads.begin(), mGamepads.end(), [&](const Gamepad* _g) {return _g->getInstanceId() == _data.which; });
            if (it != mGamepads.end()) {
                delete *it;
                mGamepads.erase(it);
            }
        }
    }

    Gamepad* Input::getGamepadFromId(int32_t _id) const {
        if (_id == -1 && !mGamepads.empty())
            return mGamepads[0];
        else {
            auto it = std::find_if(mGamepads.begin(), mGamepads.end(), [&](const Gamepad* _g) {return _g->getInstanceId() == _id; });
            if (it != mGamepads.end())
                return (*it);
        }
        return nullptr;
    }

    uint8_t Input::getButtonState(ButtonCode _button, int32_t _id) const {
        if (MX_BC_CHECK_KEYBOARD(_button)) {
            return mKeyboard->getButtonState(_button);
        }
        else if (MX_BC_CHECK_MOUSE(_button)) {
            return mMouse->getButtonState(_button);
        }
        else if (MX_BC_CHECK_GAMEPAD(_button)) {
            auto gamepad = getGamepadFromId(_id);
            if (gamepad)
                return gamepad->getButtonState(_button);
        }
        return 0;
    }

    void Input::onAnyKeyDown() {
        mAnyKeyDown = true;
    }
}
